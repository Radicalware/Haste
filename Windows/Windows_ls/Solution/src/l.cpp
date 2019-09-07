
#include "OS.h"
OS os;
#include "LibLoc.h"
#include "Core.h"

using std::cout;
using std::endl;

int help();

int main(int argc, char** argv)
{
	Core core(argc, argv);	

	if (core.options().help)
		return help();


	if (core.options().dir_style)
		core.print_dir_style();
	else
		core.print_ls_style();

	cout << cc::reset;
	return 0;
}


int help() {

	cout << R"help(

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

)help";

	return 0;
}