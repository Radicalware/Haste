# Haste

Below is the list of tools to make your day-to-day life easier with "Haste"

## MultiPlatform

$> f = Find File (v6.4.0)
    1.  Used to find filenames with multi-threading and C++ based Regex
    2.  You can limit the threadcount but default is to use all availible threads

----------------------------------------------------------------------------------------------------

$> afs = advanced file search (v6.4.0)
    1.  Used to recursively find text files with multi-threading and C++ based Regex
    2.  You can limit the threadcount but default is to use all availible threads



## Windows

$> l.exe = Windows version of ls (v5.0.0)
    1.  Used like Linux based ls
    2.  easier to read than dir because it allows you to see file type by color
        and instead of having all of the file data, you just get the file uless you 
        specify for the data.
    3.  You can sort via alphabetically (default) or by text size (to view more on a page)
    4.  dir format has color coding
    5.  Both linux style ls method and windows style dir method will seperate dirs and files


## Nix (Linux/OSX)

$> stty_shell.cpp 
    1.  Use this to help automate getting an stty remote shell on your Nix remote target
    2.  This tool requires that your target have python installed.

## Linux

$> alsr
    1. Allows for easy viewing of your aslr sate
    2. You an easily change your aslr state with this as well



# On Nix, add the following to your bash env (bashrc/bash_profile/etc)

export LIBRARY_PATH="/opt/Radicalware/Libraries/cpp/include":$LIBRARY_PATH
export LIBRARY_PATH="/opt/Radicalware/Libraries/cpp/code/Projects":$LIBRARY_PATH

export CPATH="/opt/Radicalware/Libraries/cpp/include":$CPATH
export CPATH="/opt/Radicalware/Libraries/cpp/code/Projects":$CPATH

export LD_LIBRARY_PATH="/opt/Radicalware/Tools/Release/bin":$LD_LIBRARY_PATH
export LD_RUN_PATH="/opt/Radicalware/Tools/Release/bin":$LD_RUN_PATH


