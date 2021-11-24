/*
* Copyright[2019][Joel Leagues aka Scourge]
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


#include "Options.h"
#include "Core.h"

#include "Timer.h"
#include "SYS.h"


int Help(int FnValue = 0);

int main(int argc, char** argv)
{
    Nexus<>::Start();
    RA::Timer LoTimer;
    RA::SYS Args;
    Options LoOption;
    Core LoCore(LoOption);
    
	Args.AddAlias('r', "--regex");
	Args.AddAlias('d', "--dir");
    Args.AddAlias('t', "--threads");
    Args.AddAlias('g', "--get");
	Args.AddAlias('f', "--full");
    Args.AddAlias('o', "--one");
    Args.AddAlias('c', "--case");
    Args.AddAlias('s', "--swap");
    Args.AddAlias('m', "--modify");
    Args.AddAlias('a', "--avoid");

	Args.SetArgs(argc, argv);

	if (argc == 1 || Args.Help())
		return Help();

    if (Args('g')) LoOption.SetReturnOnly(Args['g'][0]);
    if (Args('f')) LoOption.MbUseFullPath = true;
    if (Args('c')) LoOption.MoRegularExpression.MbCaseSensitive = true;
    if (Args('s')) LoOption.MbSwapSplit = true;
    if (Args('m')) LoOption.MbModify = true;

	if (!Args.HasArgs()) {
		if (argc == 2) {
            LoOption.SetRegex(argv[1]);
            LoOption.SetDirectory(RA::OS::PWD(), true);
		}
		else if (argc == 3) {
            LoOption.SetRegex(argv[1]);
            LoOption.SetDirectory(argv[2]);
		}
		else {
			return Help(1);
		}
	}
	else{
        if (Args[1][0] == '-' && !Args('r'))
            return Help(1);

        else if (!Args('r')) LoOption.SetRegex(Args[1]); // set argv[1] as the regex
        else      LoOption.SetRegex(Args['r'][0]);
		
		if (Args('d')) LoOption.SetDirectory(Args['d'][0]);
		else            LoOption.SetDirectory(RA::OS::PWD(), true);

	}

    if (Args('a'))  LoOption.SetAvoidRegex(Args['a']);

    Nexus<void> nxv;
    if (Args('t')) nxv.SetAllowedThreadCount((Args['t'][0]).ToInt());
    
	if (Args('o')) {
		LoCore.SingleCoreScan();
        LoCore.PrintFiles();
        cout << Color::Cyan;
        cout << "Single-Threaded\n";
	}
	else {
        LoCore.MultiCoreScan();
        LoCore.PrintFiles();
        cout << Color::Cyan;
        cout << "Threads Availible: " << RA::Threads::GetThreadCountAvailable() << endl;
	}
    cout << "Time: " << LoTimer << Color::Mod::Reset << endl;

    return Nexus<>::Stop();
}


int Help(int FnValue){

	cout << R"Help(
    Search is used to Search for Files

    -----------> SET COLORS ON FOR WINDOWS <--------------------------
    Set-ItemProperty HKCU:\Console VirtualTerminalLevel -Type DWORD 1

    -------------------------------------------------------------------------------
      -r     |  --regex     |  <string> Regex to search for
      -d     |  --dir       |  <string> Directory to search in
      -g     |  --get       |  <string> return types [files/dirs/both]
      -t     |  --threads   |  <int>    thread count
      -f     |  --full      |  <bool>   Show the Full Path
      -o     |  --one       |  <bool>   run under only one thread
      -c     |  --case      |  <bool>   Case-Sensitive Regex
      -s     |  --swap      |  <bool>   swap slash side
      -m     |  --modify    |  <bool>   Open files to 'Modify' 
    -------------------------------------------------------------------------------

    If no '-' are found in args are parsed as argv[x][0] 
    then the following parsed method will be uesd. 

    if (argc == 2)
        arg[1] ==  "regex"

    if (argc == 3)
        argv[1] == "regex"
        argv[2] == "directory to search in"

    if (argc > 3)
        Use the KVPs as described in the table above.

)Help";
    return Nexus<>::Stop();
}
