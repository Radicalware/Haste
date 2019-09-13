#pragma once
#include "LibLoc.h"

#include<thread>
#include<mutex>

using std::ref;

class Core
{
    xvector<xstring> m_file_lst;
    xstring m_directory;
    xstring m_rex;

	xvector<std::thread> m_threads;
	std::mutex m_mutex;

	bool m_parse_as_regex = true;
	bool m_use_full_path = false;

	static void scan_dirs(std::mutex& mx, 
		xvector<xstring>& file_lst,  const xstring& rex, // main file data
		int core_count, int zone, // multi-threading data
		size_t dir_size, bool use_full_path); // display data

public:
	void parse_as_regex(bool input);

	void set_dir(const xstring& input, bool use_pwd = false);
	void set_rex(const xstring& input);
	void init_thrads();
	void join_thrads();

	void single_core_scan();
};

