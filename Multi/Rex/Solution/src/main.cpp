
#include "Macros.h"
#include "SYS.h"
#include "OS.h"

int Help();

int main(int argc, char** argv)
{
    Begin();
    Nexus<>::Start();

    auto LoSys = RA::SYS(argc, argv);
    if(LoSys.ArgC() == 1)
        return Help();

    LoSys.AddAlias('f', "--Find");
    LoSys.AddAlias('r', "--Replace");
    LoSys.AddAlias('c', "--CaseInsensitive");
    LoSys.AddAlias('i', "--Input");
    LoSys.AddAlias('o', "--Output");

    if (LoSys.ArgCount() == 3 && RA::OS::HasFile(LoSys[1])) // FileName, Find, Replace
    {
        Begin();
        RA::OS::ReadFile(LoSys[1]).Sub(LoSys[2], LoSys[3]).Print();
        Rescue();
    }
    else if (LoSys.ArgCount() >= 1) // (stdin) | Find, Replace
    {
        Begin();
        auto LsFind    =                         (LoSys[1][0] != '-') ? LoSys[1] : ""; if (!LsFind)    LsFind    = LoSys.Key('-f').First();
        auto LsReplace = (LoSys.ArgCount() > 1 && LoSys[2][0] != '-') ? LoSys[2] : ""; if (!LsReplace) LsReplace = LoSys.Key('-r').First();

        auto LoData = (LoSys.HasVal('i'))
            ? RA::OS::ReadFile(LoSys.Key('i').First())
            : RA::OS::GetSTDIN();

        LoData = LoData
            .Split('\n')
            .ForEachThread([](xstring& Str) {  return Str.RightTrim(); })
            .Join('\n');

        if (!LoData)
        {
            cout << "<No Data>" << endl;
            return Nexus<>::Stop();
        }
        const auto LoMod = (LoSys.Has('c')) ? (RXM::ECMAScript | RXM::icase) : (RXM::ECMAScript);
        const auto Out   = LoData.Sub(LsFind, LsReplace, LoMod);
        // const auto Out   = LoData.Sub(re2::RE2(LsFind), LsReplace);

        if (LoSys.Has('o'))
        {
            auto LoOS = RA::OS();
            cout << "Saving: " << LoSys.Key('o').First() << endl;
            LoOS
                .Open(LoSys.Key('o').First(), 'w')
                .Write(Out)
                .Close();
            return Nexus<>::Stop();
        }
        Out.Print();
        Rescue();
    }
    else
        return Help();

    return Nexus<>::Stop();
    FinalRescue();
}


int Help()
{
    Begin();

    cout << R"Help(
Rex is used for finding and replacing ASCII characters.
Essentially like "sed" on "Nix".

 3 Args >> FileName, FileName, Find, Replace
 2 Args >> (STD-IN) | Find, Replace

   ----------------------------------
    -f | --Find
    -r | --Replace
    -c | --CaseInsensitive
       |
    -i | --Input  (to read)
    -o | --Output (to save)
   ----------------------------------

)Help";

    return Nexus<>::Stop();
    Rescue();
}
