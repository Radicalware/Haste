#pragma once
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

#include "LibLoc.h"
#include "File.h"

class Core
{
    xstring m_directory;
    xvector<File> m_files; // list of file data (split lines)
    xvector<xstring> m_file_lst; // list of file names in the m_directory

    File* m_piped_data = nullptr;

    xstring m_rex = '(';
    xvector<xstring*> m_avoid_lst;
    rxm::type m_icase = rxm::icase;

    bool m_use_full_path    = false;
    bool m_only_name_files  = false;
    bool m_line_tracker_on  = false;
    bool m_binary_search_on = false;
    bool m_entire           = false;

public:

    Core() {};
    ~Core();
    void piped_scan();

    void set_dir(const xstring& input, bool use_pwd = false);
    void set_rex(const xstring& input);
    void set_avoid_regex(const xvector<xstring*>& avoid_lst);
    void set_full_path_on();
    void set_only_name_on();
    void set_entire_on();

    void set_case_sensitive_on();
    void set_binary_on();

    static File scan_file(xstring& path, const Core& core, const bool search_binary);
    void multi_core_scan();
    void single_core_scan();

    void print();
    void print_divider() const;
};

