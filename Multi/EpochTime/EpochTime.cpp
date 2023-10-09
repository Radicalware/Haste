
#include "Date.h"
#include "SYS.h"


int main(int argc, char** argv)
{
    Begin();
    Nexus<>::Start();

    CliArgs.AddAlias('s', "--Seconds");
    CliArgs.AddAlias('m', "--Minutes");
    CliArgs.AddAlias('h', "--Hours");
    CliArgs.AddAlias('d', "--days");

    CliArgs.SetArgs(argc, argv);
    
    if(argc == 1)
        cout << "No Epoch Time Given (1st arg)" << endl;
    else if(argc == 2)
        cout << RA::Date(xstring(argv[1]).To64()) << endl;
    else
    {
        auto LoStart = RA::Date(xstring(argv[1]).To64());
        
        if(CliArgs.Has('s'))
            LoStart.ModSecond(CliArgs.Key('s').First().To64());
        if(CliArgs.Has('m'))
            LoStart.ModMin(CliArgs.Key('m').First().To64());
        if(CliArgs.Has('h'))
            LoStart.ModHour(CliArgs.Key('h').First().To64());
        if(CliArgs.Has('d'))
            LoStart.ModDay(CliArgs.Key('d').First().To64());
        cout << LoStart << endl;
    }

    FinalRescue();
    Nexus<>::Stop();
    return 0;
}