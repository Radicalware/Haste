
#pragma once

#include "LibLoc.h"

using uint  = unsigned int;
using usint = unsigned short int;


class Core
{
public:
	enum DirType
	{
		DIR,
		TXT,
		EXE,
		INTER
	};
	struct Options
	{
		bool dir_style = false;
		bool show_all_files = false;
		xstring dir;
	};

private:
	struct Files 
	{
		// structurally it would make sense to have the hidden identification in the 
		// Files struct but that would caues me to need to loop the *dir_items
		// and them loop again to mod the Files Struct with the hidden or not functions
		// and values. I wanted to do it in one for-loop to enhance speed at the
		// expense of beutification. 

		xstring* name;
		DirType type;
		bool hidden;
		Files();
		Files(xstring* i_name, const DirType i_type);
	};
	// ----------------------------------------
	// calculated in the constructor
	bool m_dir_style = false;
	bool m_hidden_valies = false;
	xvector<xstring> m_arg_folders;
	Options m_options;
	// ----------------------------------------

	const char* m_proj_files = R"(\.(sln|pro)$)";
	const char* m_exe_files = R"(\.(ps1|rb|com|exe|bat|cmd|vbs|vbe|js|jse|wsf|wsh|msc|py|pyw|cpl)$)";
	xstring m_exe_ext;

	xvector<Core::Files> m_directories;
	xvector<Core::Files> m_files;

	xvector<xstring> m_str_files;
	xvector<xstring> m_str_directories;
	xstring _example;

	usint m_space_size = 2;
	int m_console_width = os.console_size()[0];
	xstring empty_str;
	bool m_view_all_files = false;
public:

	Core(int argc, char** argv);  // called when initialized
	~Core(); // called when destroyed

private:
	bool add_dir_items(xstring& item);
	void trim(xstring& item);
	bool system_hidden(const xstring& item);
	bool user_hidden(const xstring& item);

	void print_File_List(const xvector<Files>& Files_Vec);
	void continued_print_files(xvector<Files>& dir_lst);

public:
	void dup_char_out(size_t count, char chr);
	void print_ls_style();
	void print_dir_style();
	Core::Options options();
};


