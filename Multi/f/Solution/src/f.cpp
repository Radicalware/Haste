#include "LibLoc.h"
#include "Core.h"


int help(int val = 0);

int main(int argc, char** argv)
{
    SYS sys;

	sys.alias('r', "--regex");
	sys.alias('d', "--dir");
    sys.alias('t', "--threads");
    sys.alias('g', "--get");
	sys.alias('f', "--full");
    sys.alias('o', "--one");
    sys.alias('c', "--case");
    sys.alias('s', "--swap");

	sys.set_args(argc, argv);

	if (argc == 1 || sys.help())
		return help();

    Core core;

    if (sys('f'))
        core.set_full_path();

    if (sys('s'))
        core.swap_split();
	if (!sys.key_used()) {
		if (argc == 2) {
			core.set_rex(argv[1]);
			core.set_dir(OS::PWD(), true);
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
        if (sys[1][0] == '-' && !sys('r'))
            return help(1);
        else if (!sys('r'))
            core.set_rex(sys[1]); // set argv[1] as the regex
        else
            core.set_rex(*sys['r'][0]);
		
		if (sys('d'))
			core.set_dir(*sys['d'][0]);
		else
			core.set_dir(OS::PWD(), true);

        if (sys('c'))
            core.set_case_sensitive();
	}

    if (sys('g'))
        core.return_only(*sys['g'][0]);

    Nexus<void> nxv;
    if (sys('t')) 
        nxv.Set_Thread_Count((*sys['t'][0]).to_int());
    
    
    Timer t;
	if (sys('o')) {
		core.single_core_scan();
        core.print_files();
        cout << cc::cyan;
        cout << "Single-Threaded\n";
	}
	else {
        core.multi_core_scan();
        core.print_files();
        cout << cc::cyan;
        cout << "Threads Availible: " << NX_Threads::Thread_Count_Available() << endl;
	}
    cout << "Time: " << t << cc::reset << endl;

    return 0;
}


int help(int val){

	cout << R"help(
    f is used to Find Files
    f used as f.exe

    -----------> SET COLORS ON FOR WINDOWS <-------------------------
    Set-ItemProperty HKCU:\Console VirtualTerminalLevel -Type DWORD 1

    -------------------------------------------------------------------------------
      Key    |   Key (long) |  Value
    -------------------------------------------------------------------------------
      -r     |  --regex     |  <string> Regex to search for
      -d     |  --dir       |  <string> Directory to search in
      -g     |  --get       |  <string> return types [files/dirs/both]
      -t     |  --threads   |  <int>    thread count
      -f     |  --full      |  <bool>   Show the Full Path
      -o     |  --one       |  <bool>   run under only one thread
      -c     |  --case      |  <bool>   Case-Sensitive Regex
      -s     |  --swap      |  <bool>   swap slash side
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

)help";
    return 0;
}
