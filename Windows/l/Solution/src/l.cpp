#include "Core.h"

using std::cout;
using std::endl;

int Help();

int main(int argc, char** argv)
{
	Nexus<>::Start();
	Core LoCore(argc, argv);

	if (!LoCore.MbExists) {
		cout << "'" << LoCore.GetOptions().MsDir.ToRed().ResetColor() << "' does not exist!\n";
		return 0;
	}

	if (LoCore.GetOptions().MbHelp)
		return Help();

	if (LoCore.GetOptions().MbDirStyle)
		LoCore.PrintDirStyle();
	else
		LoCore.PrintLsStyle();

	cout << CC::Reset;
	Nexus<>::Stop();
	return 0;
}


int Help() {

	cout << R"Help(

	This tool 'l.exe' execute as 'l'
	is to replicate the main functionality of ls from Nix (l for short)
	
	 Arguments
	---------------------
	 short  |   long
	---------------------
	 -a     |   --all
	 -s     |   --size
	 -d     |   --dir
	---------------------

	--all >> Will show all files. By default, hidden system files,
	and files that start with a "." or a "_" will be hidden so you
	can focus on what matters most. 

	--size >> Oftentimes in windows you will have files that are
	significatnly longer than others. --size sorts by size to 
	use more whitespace on the screen so you can see more at one time.
	By default, The sorting is done alphabetically.

	--dir >> Uses the standard windows Dir style and then color-codes
	the text. It also by default hides files that would otherwise
	be shown with the --all argument.

)Help";

	Nexus<>::Stop();
	return 0;
}
