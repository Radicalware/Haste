#pragma once
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

#include "LibLoc.h"
#include "File.h"

extern OS  os;  // Operating System Object
extern SYS sys; // Args Object

class Core
{
    xvector<File> m_files;

    xvector<xstring> m_file_lst;
    xstring m_directory;

    xstring m_rex;
    rxm::type m_icase = rxm::icase;

    xvector<std::thread> m_threads;
    std::mutex m_mutex;

    xvector<xstring> m_found_files;

    bool m_use_full_path   = false;
    bool m_line_tracker_on = false;
    bool m_binary_search   = false;

public:
    Core core() {};

    void set_dir(const xstring& input, bool use_pwd = false);
    void set_rex(const xstring& input);

    void set_case_sensitive_on();
    void set_binary_on();

    static File scan_file(xstring& path, const Core& core, const OS& osi, const bool search_binary);
    void multi_core_scan();
    void single_core_scan();

    void print();
    void print_divider() const;
};

