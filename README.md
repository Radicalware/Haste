
# Bring "HASTE" to your life with these Tools!!  


## MultiPlatform

Search.exe = Find Files with Regex
    a.  Used to find filenames with multi-threading and C++ based Regex  
    b.  You can limit the threadcount but default is to use all availible threads  

Scan.exe = Scan Files for Regex
    a.  Used to recursively find text files with multi-threading and C++ based Regex  
    b.  You can limit the threadcount but default is to use all availible threads  

Cipher.exe = Encrypt/Decrypt AES Pass-Codes

## Windows

l.exe = Windows version of ls
    a.  Used like Linux based ls  
    b.  easier to read than dir because it allows you to see file type by color  
    c.  You can sort via alphabetically (default) or by text size (to view more on a page)  
    d.  dir format has color coding  
    e.  Both linux style ls method and windows style dir method will seperate dirs and files  

## Nix (Linux/OSX)

stty_shell.cpp  
    a.  Use this to help automate getting an stty remote shell on your Nix remote target  
    b.  This tool requires that your target have python installed.  

## Linux

alsr  
    a. Allows for easy viewing of your aslr sate  
    b. You an easily change your aslr state with this as well  



****************************************************************************************************

## On Nix, add the following to your bash env (bashrc/bash_profile/etc)

export PATH="/opt/Radicalware/Applications/Build/Release":$PATH

export LIBRARY_PATH="/usr/local/include":$LIBRARY_PATH
export LIBRARY_PATH="/opt/Radicalware/Libraries/cpp/include":$LIBRARY_PATH
export LIBRARY_PATH="/opt/Radicalware/Libraries/cpp/code/Projects":$LIBRARY_PATH

export CPATH="/opt/Radicalware/Libraries/cpp/include":$CPATH
export CPATH="/opt/Radicalware/Libraries/cpp/code/Projects":$CPATH

export LD_LIBRARY_PATH="/opt/Radicalware/Tools/Release/bin":$LD_LIBRARY_PATH
export LD_RUN_PATH="/opt/Radicalware/Tools/Release/bin":$LD_RUN_PATH

