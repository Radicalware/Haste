
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

#include "OS.h"
#include "SYS.h"

OS os;
SYS sys;

#include "LibLoc.h"
#include "Core.h"



int help(int ret_err) {
    cout << R"(

    afs is used to Find text in Files Recursivly
    afs used as afs.exe

    ------------------------------------------------------------------
      Key    |   Key (long) |  Value
    ------------------------------------------------------------------
      -r     |  --regex     |  (str)  Regex to search for
      -d     |  --dir       |  (str)  Directory to search in
      -f     |  --full      |  (bool) Show the Full Path
      -o     |  --one       |  (bool) run under only one thread
      -t     |  --threads   |  (int)  thread count
      -c     |  --case      |  (bool) Case-Sensitive Regex
      -b     |  --binary    |  (bool) Search Binary Files 
    ------------------------------------------------------------------

    If no '-' are found in args are parsed as argv[x][0] 
    then the following parsed method will be uesd. 

    if (argc == 2)
        arg[1] ==  "regex"

    if (argc == 3)
        argv[1] == "regex"
        argv[2] == "directory to search in"

    if (argc > 3)
        Use the KVPs as described in the table above.

)";
    return ret_err;
}

int main(int argc, char** argv) 
{	
    Timer t;
    sys.alias('r', "--regex");
    sys.alias('d', "--dir");
    sys.alias('f', "--full");
    sys.alias('o', "--one");
    sys.alias('t', "--threads");
    sys.alias('c', "--case");
    sys.alias('b', "--binary");
    sys.set_args(argc, argv);

    if (sys.help()) 
        return help(0);
    
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
    else {
        if (!sys('r'))
            return help(1);

        if (sys('d'))
            core.set_dir(*sys['d'][0]);
        else
            core.set_dir(os.pwd(), true);

        core.set_rex(*sys['r'][0]);
    }

    if (sys('c'))
        core.set_case_sensitive_on();

    if (sys('b')) 
        core.set_binary_on();
    
    if (sys('t'))
        CPU_Threads::set_thread_count((*sys['t'][0]).to_int());

    core.print_divider();
    if (sys('o')) {
        core.single_core_scan();
        core.print();
        cout << cc::cyan << "Single-Threaded\n" << cc::reset;
    }
    else {
        core.multi_core_scan();
        core.print();
        cout << cc::cyan << "Threads Availible: " << CPU_Threads::threads_available() << cc::reset << endl;
    }
    cout << cc::cyan << "Time: " << t << cc::reset << endl;
    core.print_divider();
	
	return 0;
}

