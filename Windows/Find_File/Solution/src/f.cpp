
#include "OS.h"
#include "SYS.h"

OS  os;
SYS sys;

#include "LibLoc.h"
#include "Core.h"

#include "Timer.h"

int help(int val = 0);

int main(int argc, char** argv)
{
	sys.alias('r', "--regex");
	sys.alias('d', "--dir");
	sys.alias('f', "--full");
    sys.alias('o', "--one");
    sys.alias('t', "--threads");
	sys.set_args(argc, argv);

	if (argc == 1 || sys.help())
		return help();

    Core core;
	if (!sys.key_used()) {
		if (argc == 2) {
			core.set_rex(argv[1]);
			core.set_dir(os.pwd(), true);
		}
		else if (argc == 3) {
			core.set_rex(argv[1]);
			core.set_dir(argv[2]);
		}
		else {
			help(1);
		}
	}
	else{
		if (!sys('r'))
			return help(1);
		
		if (sys('d'))
			core.set_dir(*sys['d'][0]);
		else
			core.set_dir(os.pwd(), true);

		core.set_rex(*sys['r'][0]);
	}
    Nexus<void> nxv;
    if (sys.argv().has("--threads")) {
        nxv.set_thread_count((*sys['t'][0]).to_int());
    }
    
    Timer t;
	if (sys.argv().has("--one")) {
		core.single_core_scan();
        cout << cc::reset;
        cout << "Single-Threaded\n";
	}
	else {
        core.gather_files(); core.find_matching_files();
        cout << cc::reset;
        cout << "Threads Availible: " << CPU_Threads::threads_available() << endl;
	}
    cout << "Time: " << t << endl;

    return 0;
}


int help(int val){

	cout << R"(
    f is used to Find Files
    f used as f.exe

    -------------------------------------------------------
      Key    |   Key (long) |  Value
    -------------------------------------------------------
      -r     |  --regex     |  Regex to search for
      -d     |  --dir       |  Directory to search in
      -f     |  --full      |  Show the Full Path
      -o     |  --one       |  run under only one thread
    -------------------------------------------------------

    If no '-' are found in args are parsed as argv[x][0] 
    then the following parsed method will be uesd. 

    if argc == 2 
    arg[1] == "regex to search for in your current path

    if Argc == 3
    argv[1] == "regex to search for in your current path"
    argv[2] == "directory to search in"


    )";

    return 0;
}
