
#include "OS.h"
OS os;
#include "LibLoc.h"
#include "Core.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
	// I decided to not use the SYS objct here becaues I wanted to make
	// the list output run as fast as possible.

	Core core(argc, argv);	

	if (core.options().dir_style)
		core.print_dir_style();
	else
		core.print_ls_style();

	cout << cc::reset;
	return 0;
}
