#pragma once

#include "LibLoc.h"

struct File
{
    struct Splits // Splits up each line of a file
    {
        xvector<xstring> splits;
        unsigned long int line_num = 0;    
        static const double max_line_len;
        Splits();
        Splits(const Splits& split);
        xstring& operator[](size_t val);
    };

    File();
    ~File();
    File(const File& file);
    File(const xstring& i_path, bool ibinary_search_on);
    void operator=(const File& file);

    xstring path;
    xstring data;
    xvector<File::Splits> lines;

    bool binary  = false;
    bool matches = false;
    bool binary_search_on = false;
    xstring err;
    //xstring* err;

    void print(const xstring& rex);
    void print_divider() const;
};

