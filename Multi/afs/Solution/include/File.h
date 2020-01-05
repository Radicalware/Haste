#pragma once

#include "OS.h"

#include "xvector.h"
#include "xstring.h"

#include <iostream>
using std::cout;
using std::endl;

struct File
{
    struct Splits // Splits up each line of a file
    {
        xvector<xstring> splits;
        unsigned long int line_num = 0;    
        static const double max_line_len;
        Splits();
        Splits(const xstring& line);
        Splits(const Splits& split);
    };

    File();
    ~File();
    File(const File& file);
    File(const xstring& i_path, bool ibinary_search_on);
    void operator=(const File& file);

    xstring path;
    xstring data;
    xvector<xstring> lines;

    bool piped_data = false;
    bool binary  = false;
    bool matches = false;
    bool binary_search_on = false;
    bool indent = false;
    xstring err;

    void print();
    void print_divider() const;
};

