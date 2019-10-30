#pragma once
#include "LibLoc.h"

#include<thread>
#include<mutex>

using std::ref;

class Core
{
    struct Splits
    {
        xvector<xstring> rex_on;
        xvector<xstring> rex_off;
    };

    xvector<xstring> m_file_lst;
    xstring m_directory;
    xstring m_rex;

	xvector<std::thread> m_threads;
	std::mutex m_mutex;

    xvector<xstring> m_found_files;

	bool m_parse_as_regex = true;
	bool m_use_full_path = false;

	static void scan_dirs(std::mutex& mx, 
		xvector<xstring>& file_lst,  const xstring& rex, // main file data
		int core_count, int zone, // multi-threading data
		size_t dir_size, bool use_full_path); // display data

public:
    Core core() {};
	void parse_as_regex(bool input);

	void set_dir(const xstring& input, bool use_pwd = false);
	void set_rex(const xstring& input);

    void gather_files();
    void find_matching_files();

	void init_thrads();
	void join_thrads();

	void single_core_scan();

};

