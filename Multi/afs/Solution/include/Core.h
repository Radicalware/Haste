#pragma once
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

#include "File.h"
#include "Options.h"

class Core
{
    const Options& m_option; // based on user input

    xvector<File> m_files; // list of file data (split lines)
    xvector<xstring> m_file_lst; // list of file names in the m_directory

    File* m_piped_data = nullptr;
    const RE2 m_backslash_rex = R"(\\)";

public:
    Core(const Options& options);
    ~Core();

    void piped_scan();
    static File scan_file(xstring& path, const Core& core, const bool search_binary);

    void multi_core_scan();
    void single_core_scan();

    void print();
    void print_divider() const;
};

