#pragma once
#include "LibLoc.h"

#include<thread>
#include<mutex>

class Core
{
    struct Splits
    {
        xvector<xstring> rex_on;
        xvector<xstring> rex_off;
        bool not_found = true;
    };

    xvector<xstring> m_file_lst;
    xstring m_directory;

    xstring m_rex = '(';
    rxm::type m_icase = rxm::icase;

	xvector<std::thread> m_threads;
	std::mutex m_mutex;

    xvector<xstring> m_found_files;

	bool m_parse_as_regex = true;
	bool m_use_full_path = false;
    bool m_swap_split = false;
    
public:
    Core core() {};
	void parse_as_regex(bool input);
    void set_case_sensitive();

	void set_dir(const xstring& input, bool use_pwd = false);
	void set_rex(const xstring& input);
    void swap_split();

    void gather_files();
    void find_matching_files();

	void single_core_scan();

};

