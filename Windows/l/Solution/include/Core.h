
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
		xstring dir;
		bool dir_style      = false;
		bool show_all_files = false;
		bool sort_by_size   = false;

		bool help           = false;
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
	Options m_options;
	// ----------------------------------------

	const char* m_proj_files = R"(\.(sln|pro)$)";
	const char* m_exe_files  = R"(\.(ps1|rb|com|exe|EXE|bat|cmd|vbs|vbe|js|jse|wsf|wsh|msc|py|pyw|cpl)([\"\s]?)$)";
	xstring m_exe_ext;

    Nexus<xvector<xstring>> m_nex_vec;

	xvector<Core::Files> m_directories;
	xvector<Core::Files> m_files;

	xvector<xstring> m_str_files;
    xvector<xstring> m_str_directories;
	xstring _example;

	usint m_space_size = 2;
	int m_console_width = OS::Console_Size()[0];
	xstring empty_str;
	bool m_view_all_files = false;

public:
	Core(int argc, char** argv); 
	~Core(); 
	bool exists = false;
private:
	bool add_dir_items(xstring& item);
	void trim(xstring& item);
	void sort_files(xvector<Files>& file_lst);

	bool system_hidden(const xstring& item);
	bool user_hidden(const xstring& item);

	void continued_print_files(xvector<Files>& dir_lst);
	void print_ls_style_size(xvector<Files*>& pfiles);
	void print_ls_style_alph(xvector<Files*>& pfiles);

	void dir_ptr_out(const Files* item) const;
	void dir_vals_out(const xvector<Files>& dir_lst) const;

	void add_word_space(size_t count) const;
public:

	void print_ls_style();
	void print_dir_style();

	Core::Options options() const;
};
