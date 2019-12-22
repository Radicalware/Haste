#pragma once
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)

#include "LibLoc.h"

class Core
{
    char m_find_mod1 = 'f';
    char m_find_mod2 = 'f';

    xvector<xstring> m_file_lst;
    xstring m_directory;

    xstring m_rex = '(';
    rxm::type m_icase = rxm::icase;

	xvector<std::thread> m_threads;
	std::mutex m_mutex;

    xvector<xstring> m_found_files;

	bool m_use_full_path = false;
    bool m_swap_split = false;

public:
    Core core() {};
    void set_case_sensitive();

	void set_dir(const xstring& input, bool use_pwd = false);
	void set_rex(const xstring& input);
    void set_full_path();
    void swap_split();
    void return_only(const xstring& ret_only);

    static xstring find_matching_files(xstring& item, Core& core);

    void multi_core_scan();
    void single_core_scan();

    void print_files();
};

