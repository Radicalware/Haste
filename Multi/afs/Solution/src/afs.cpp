
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

// DEBUG ARGS: -r rxm::icase -d C:\source\Tools\afs -o

#include "LibLoc.h"
#include "Core.h"

int help(int ret_err) {
    cout << R"(

    afs is used to Find text in Files Recursivly
    afs used as afs.exe

    ------------------------------------------------------------------
      Key    |   Key (long) |  Value
    ------------------------------------------------------------------
      REQUIRED
    ------------------------------------------------------------------
      -r     |  --regex     |  (str)   Regex To Search For
    ------------------------------------------------------------------
      USUAL ARGUMENTS
    ------------------------------------------------------------------
      -d     |  --dir       |  (str)   Directory To Search In
      -t     |  --threads   |  (int)   Set Thread Count
      -a     |  --avoid     |  (array) avoid these regex per line
      -f     |  --full      |  (bool)  Show the Full Path
      -c     |  --case      |  (bool)  Case-Sensitive Regex
      -b     |  --binary    |  (bool)  Search Binary Files 
      -n     |  --name      |  (bool)  Only Name the Files Found
    ------------------------------------------------------------------
       THESE CHANGE THE STANDARD PROCEDURE
    ------------------------------------------------------------------
      -o     |  --one       |  (bool)  Run Only Under One Thread
    ------------------------------------------------------------------
      -p     |  --pipe      |  (bool)  Consume Piped Data
      -e     |  --entire    |  (bool)  If Data Is Piped, Show All Data
    ------------------------------------------------------------------

    If no '-' are found in args are parsed as argv[x][0] 
    then the following parsed method will be uesd. 

    if (argc == 2) // requires stdin
        argv[1] =  "regex"

    if (argc == 3)
        argv[1] = "directory to search in"
        argv[2] = "regex"

    if (argc > 3)
        Use the KVPs as described in the table above.

)";
    return ret_err;
}

int main(int argc, char** argv) 
{	
    Timer t;
    SYS sys;
    sys.alias('r', "--regex"); // -- always required

    sys.alias('d', "--dir");   // -- usual bools
    sys.alias('t', "--threads");
    sys.alias('a', "--avoid");
    sys.alias('f', "--full");
    sys.alias('c', "--case");
    sys.alias('b', "--binary");
    sys.alias('n', "--name");

    sys.alias('o', "--one");    // -- setting only one thread path is not the same as -t 1


    sys.alias('p', "--pipe");   // -- piped path
    sys.alias('e', "--entire");

    sys.set_args(argc, argv);

    Core core;
    if (sys.help()) 
        return help(0);

    auto find_rex_arg = [&core, &sys]() -> void { // note: argv[0] is the program path
        if (sys.argc() == 2 && !sys('r')) // no --regex && 1 prog args == set argv[1] as the regex
            core.set_rex(sys[1]);
        else if (sys.argc() > 2 && !sys('r'))  // no --regex && over 1 prog arg == set argv[2] as the regex
            core.set_rex(sys[2]);
        else
            core.set_rex(*sys['r'][0]);
    };

    auto set_bools = [&core, &sys]() -> void {

        if (!sys('d')) core.set_dir(OS::PWD(), true);
        else           core.set_dir(*sys['d'][0]);

        if (sys('t')) NX_Threads::Set_Thread_Count((*sys['t'][0]).to_int());
        if (sys('a')) core.set_avoid_regex(sys['a']);
        if (sys('f')) core.set_full_path_on();
        if (sys('c')) core.set_case_sensitive_on();
        if (sys('b')) core.set_binary_on();
        if (sys('n')) core.set_only_name_on();
    };

    // use piped scan if there is ony one arg and it is not a key
    if ((sys.argc() == 2 && sys[1][0] != '-') || sys('p'))
    {
        find_rex_arg();
        set_bools();

        core.piped_scan();
        core.print();
        return 0; // << -------- return ----------------------
    }

    else if (!sys.key_used()) {
        if (argc == 2) {
            core.set_rex(argv[1]);
            core.set_dir(OS::PWD(), true);
        }
        else if (argc == 3) {
            core.set_dir(argv[1]);
            core.set_rex(argv[2]);
        }
        else {
            return help(1); // << -------- return -----------
        }
    }
    else 
    {
        find_rex_arg();
        set_bools();
    }

    core.print_divider();
    if (sys('o')) {
        core.single_core_scan();
        core.print();
        cout << cc::cyan << "Single-Threaded\n" << cc::reset;
    }
    else {
        core.multi_core_scan();
        core.print();
        cout << cc::cyan << "Threads Availible: " << NX_Threads::Thread_Count_Available() << cc::reset << endl;
    }
    cout << cc::cyan << "Time: " << t << cc::reset << endl;
    core.print_divider();
	
	return 0;
}


