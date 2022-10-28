
/*
* Copyright[2021][Joel Leagues aka Scourge]
* Scourge /at\ protonmail /dot\ com
* www.Radicalware.com
* https://www.youtube.com/channel/UCivwmYxoOdDT3GmDnD0CfQA/playlists
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http ://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "Macros.h"
#include "AES.h"
#include "Timer.h"
#include "SYS.h"
#include "OS.h"

int Help(int FnReturnError) {
    cout << R"(
    
     APP = AES.exe
    ------------------------------------------------------------------
      REQUIRED
    ------------------------------------------------------------------
      -e     |  --Encode    |  (str)   Text to Encode
      -d     |  --Decode    |  (str)   Text to Decode
    ------------------------------------------------------------------
       Key/Tag/IVs are optional for encoding (in favor of randoms)
       Decoding always needs these inputs.
    ------------------------------------------------------------------
      -k     |  --Key       |  (str)   Key
      -t     |  --Tag       |  (str)   Tag
      -a     |  --AAD       |  (str)   AAD
      -i     |  --IV        |  (str)   IV
    ------------------------------------------------------------------
       Optionals
    ------------------------------------------------------------------
             |              |  (ARR[int,str])  Output format as Hex
      -x     |  --Hex       |   > Int > Number of bytes per new line
             |              |   > str > Line Trail
    ------------------------------------------------------------------
      -v     |  --Verbose   |  (bool) Show Key/Tag/IV/ADD
      -o     |  --Output    |  (str)  OutputFile
    ------------------------------------------------------------------

)";
    return Nexus<>::Stop();
}


int PrintExit(const xstring& ExitText)
{
    cout << ExitText << endl;
    return Nexus<>::Stop();
}

int main(int argc, char** argv) 
{   
    Begin();

    RA::Timer Timer;
    Nexus<>::Start();

    RA::SYS Args;

    Args.AddAlias('e', "--Encode");
    Args.AddAlias('d', "--Decode");

    Args.AddAlias('k', "--Key");
    Args.AddAlias('t', "--Tag");
    Args.AddAlias('a', "--AAD");
    Args.AddAlias('i', "--IV");

    Args.AddAlias('x', "--Hex");
    Args.AddAlias('v', "--Verbose");

    Args.SetArgs(argc, argv);

    if (Args.Help())
        return Help(0);

    cout << '\n';
    RA::AES AES;
    if (Args.Has('e') && Args.Has('d'))
        return PrintExit("You can't have both 'Encrypt' and 'Decrypt' at the same time!");
    else if (Args.HasVal('e'))
        AES.SetPlainText(Args.GetVal('e'));
    else if (Args.HasVal('d'))
        if (Args.GetVal('d').IsByteCode())
            AES.SetCipherText(Args.GetVal('d').FromByteCodeToASCII());
        else
            return PrintExit("Cipher Text is not in Byte-Code Format: \"\\xAA\\xBB\\xCC\\xDD\"");
    else                           
        return PrintExit("Nothing added to Encrypt or Decrypt!");

    if (Args.HasVal('e'))
        AES.SetAllRandomValues();
    else if (!Args.HasVal('t') || !Args.HasVal('k') || !Args.HasVal('i') || !Args.HasVal('a'))
        return PrintExit("Missing Values for Decryption!");

    if (Args.HasVal('t'))
        if (Args.GetVal('t').IsByteCode())
            AES.SetTag(Args.GetVal('t').FromByteCodeToASCII());
        else
            PrintExit("Tag is not byte-string encoded: " + Args.GetVal('t'));

    if (Args.HasVal('k'))
        if (Args.GetVal('k').IsByteCode())
            AES.SetKey(Args.GetVal('k').FromByteCodeToASCII());
        else
            PrintExit("Key is not byte-string encoded: " + Args.GetVal('k'));

    if (Args.HasVal('i'))
        if (Args.GetVal('i').IsByteCode())
            AES.SetIV(Args.GetVal('i').FromByteCodeToASCII());
        else
            PrintExit("IV is not byte-string encoded: " + Args.GetVal('i'));


    if (Args.HasVal('a'))
        if (Args.GetVal('a').IsByteCode())
            AES.SetAAD(Args.GetVal('a').FromByteCodeToASCII());
        else
            PrintExit("AAD is not byte-string encoded: " + Args.GetVal('a'));

    std::ostringstream SS;

    if (Args.Has('e'))
        AES.Encrypt();
    else
        AES.Decrypt();

    if (Args.Has('x'))
    {
        xstring ByteStr; 
        xvector<xstring> SplitCode;
        if (Args.Has('e'))
            ByteStr   = AES.GetCipherTextByteCode();
        else
            ByteStr   = AES.GetPlainText().ToByteCode();

        if (Args.Key('x').HasRange(1))
            SplitCode = ByteStr
            .Split(Args.Key('x')[0].To64() * 4)
            .ForEach([](auto& Str) { return '"' + Str + '"'; });

        if (Args.Key('x').HasRange(2))
            ByteStr   = SplitCode.Join(' ' + Args.Key('x')[1] + '\n');
        else
            ByteStr   = SplitCode.Join('\n');
        SS << ByteStr << '\n';
    }
    else {
        if (Args.Has('e')) SS << "Cipher  = \"" << AES.GetCipherTextByteCode() << "\"\n";
        else /* Decrypt */ SS << "Decoded = \"" << AES.GetPlainText() << "\"\n";
    }

    if (Args.Has('v'))
    {
        SS << "\n";
        SS << "Tag     = \"" << AES.GetTag().ToByteCode() << "\"\n";
        SS << "Key     = \"" << AES.GetKey().ToByteCode() << "\"\n";
        SS << "IV      = \"" << AES.GetIV() .ToByteCode() << "\"\n";
        SS << "AAD     = \"" << AES.GetAAD().ToByteCode() << "\"\n";
    }


    cout << SS.str() << endl;
    if (Args.Has('o') && Args.Key('o').HasRange(1))
    {
        RA::OS OS;
        OS.Open(Args.Key('o')[0], 'w').Write(SS).Close();
    }

    cout << "Time: " << Timer.GetElapsedTime() << endl;

    RescuePrint();
    return Nexus<>::Stop();
}

