
// stty_shell
// Version 1.2.0

// g++ stty_shell.cpp -o stty_shell -lOS -lSYS -lcc -lre

/*
* Copyright[2018][Joel Leagues aka Scourge]
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



#include<iostream>
#include<string>
#include<vector>
#include<sys/wait.h> // for wait() 
#include<unistd.h>   // for fork() 

// ----------------------------------------------------
// The following can be downloaded from 
// https://github.com/Radicalware

#include "OS.h"    // by: Joel leagues aka Scouge
#include "SYS.h"   // by: Joel leagues aka Scouge
extern OS os;
extern SYS sys;

#include "re.h"    // by: Joel leagues aka Scouge
#include "cc.h"    // by: Ihor Kalnytskyi
// ----------------------------------------------------

using std::cout;
using std::endl;
using std::string;
using std::vector;

void help(int ret_err = 0);


bool shell_execute(const string& command, int out_text){
    bool done = false;

    pid_t id1 = fork();
    pid_t id2 = fork();

    if (id1 > 0 && id2 > 0) { 
        wait(NULL); 
        wait(NULL); 

        if (out_text == 1){
            cout << cc::green << "\nPython STTY Sent to Clipboard!!\n" << cc::white;
            cout << "1. paste your content (now in clipboard) into the victim's terminal\n";
            cout << "2. hit 'ctrl+z' in your victim's terminal\n";
            cout << "3. come back to this prompt and hit 'enter' for the next step.\n";
        }else if(out_text == 2){
            cout << cc::green << "You have the 2nd clip!!" << cc::white << endl;
            cout << "1. Just go back to your other terminal and paste the last \n";
            cout << "piece of data. No need to hit 'fg' that is in your clipboard\n";
        }else if(out_text == 3){
            cout << cc::green << "You have the last clip!!" << cc::white << endl;
            cout << "Just paste in your colors and your terminal will be easy to read!!\n";
            cout << cc::red << "\npress return to exit\n";
        }

        char tmp = 'n';
        while(tmp == 'n'){
            std::cin.get(tmp);
        }        
        done = true;
        kill(id1, SIGKILL);
        kill(id2, SIGKILL);
    } 
    else if (id1 == 0 && id2 > 0) { 
        wait(NULL); 
        os.popen(command + "| xclip -i  > /dev/null 2>&1");   
    } 
    else if (id1 > 0 && id2 == 0) { 
        os.popen(command + "| xclip -selection clipboard  > /dev/null 2>&1"); 
    } 

    return done;
}


int main(int argc, char** argv){

    sys.set_args(argc, argv);

    if(sys.help()){ help(0); }


    if(!re::scan("^Usage: xclip ", os("xclip -h 2>&1"))){
        cout << "You do not have xclip in your $PATH\n";
        exit(1);
    }

    std::string python_shell = R"pty(echo "python -c 'import pty;pty.spawn(\"/bin/bash\")'; 
                echo

                ")pty";


    while(!shell_execute(python_shell, 1)){exit(0);}

    string term_size = (R"pty(   
        echo "

        stty raw -echo

        fg

        reset

        clear


        )pty");



    string cols = re::sub("\n","",(os("tput cols")));
    string rows = re::sub("\n","",(os("tput lines")));

    //string cols = std::to_string(std::stoi(os("tput cols" )));
    //string rows = std::to_string(std::stoi(os("tput lines")));



    term_size += "export SHELL=bash\n";
    //term_size += "TERM=xterm256-color\n";
    term_size += "stty rows " + rows + " columns " + cols + '\n';



    term_size += "clear\n\n\n\n\"";

    while(!shell_execute(term_size, 2)){exit(0);}


    string color_config = string(R"term_colors(

echo "
# enable color support of ls and also add handy aliases
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval \"\$(dircolors -b ~/.dircolors)\" || eval \"\$(dircolors -b)\"
    alias ls='ls --color=auto'
    alias dir='dir --color=auto'
    alias vdir='vdir --color=auto'

    alias grep='grep --color=auto'
    alias fgrep='fgrep --color=auto'
    alias egrep='egrep --color=auto'
fi

export PS1='\\\[\\\033[1;31m\\\][\\\[\\\033[33;1m\\\]\\\w\\\[\\\033[31;1m\\\]]\\\[\\\033[m\\\]\\\n\\\[\\\033[1;31m\\\]\\\u\\\[\\\033[m\\\]@\\\[\\\033[1;31m\\\]\\\h:\\\[\\\033[31;1m\\\]\\$\\\[\\\033[1;m\\\] '

export CLICOLOR=1
# todo: make an include option for this later
# export LS_COLORS='di=1;36:ln=35:so=32:pi=33:ex=1;32:bd=34;46:cd=34;43:su=30;41:sg=30;46:tw=30;42:ow=30;43'

ls

" )term_colors");


    while(!shell_execute(color_config, 3)){exit(0);}

    return 0;
}

void help(int ret_err){
    cout << R"(

    Upgrade to a full STTY Shell

    Easy as following the on-screen instructions.

    REQUIREMENTS: $PATH to the tool "xclip"

    ./stty_shell


)";
    exit(ret_err);
}
