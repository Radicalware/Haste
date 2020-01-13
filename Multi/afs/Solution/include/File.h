#pragma once

#include "OS.h"

#include "xvector.h"
#include "xstring.h"

#include <iostream>
using std::cout;
using std::endl;

struct File
{
    const RE2 m_backslash_rex = R"((\\\\))";

    xstring path;
    xstring data;
    xvector<xstring> lines;
    xstring err;

    bool piped_data = false;
    bool binary = false;
    bool matches = false;
    bool binary_search_on = false;
    bool indent = false;

    File();
    ~File();
    File(const File& file);
    File(const xstring& i_path, bool ibinary_search_on);
    void operator=(const File& file);

    void print();
    void print_divider() const;
};

