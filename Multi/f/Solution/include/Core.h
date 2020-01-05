#pragma once
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)

#include "Options.h"

class Core
{
    const Options& m_option; // based on user input

    xvector<xstring> m_file_lst;

	xvector<std::thread> m_threads;
	std::mutex m_mutex;

    xvector<xstring> m_found_files;

public:
    Core(const Options& options);

    static xstring find_matching_files(xstring& item, Core& core);

    void multi_core_scan();
    void single_core_scan();

    void print_files();
};

