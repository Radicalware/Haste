#include "Core.h"
#include "Timer.h"

#include <windows.h>

#define  NOMINMAX

using std::cout;
using std::endl;

int Help();
xstring GetSTDIN();


int main(int argc, char** argv)
{
	Begin();
	Nexus<>::Start();

	Core LoCore(argc, argv);
	LoCore.AquireData();
	LoCore.ParseRegexFind();
	LoCore.Print();	

	FinalRescue();
	return Nexus<>::Stop();
}


int Help() {

	cout << R"Help(

	This tool 'rex.exe' execute as 'rex'
	
	 Usage: Pipe
	------------------------------------------
	 cat file.txt | rex find
	------------------------------------------
	 cat file.txt | rex find replace
	------------------------------------------


	 Usage: File Argument
	------------------------------------------
	 rex file.txt find
	------------------------------------------
	 rex file.txt find replace
	------------------------------------------

	
	 Options
	------------------------------------------
	 c | --Colorless
	------------------------------------------

)Help";

	Nexus<>::Stop();
	return 0;
}


