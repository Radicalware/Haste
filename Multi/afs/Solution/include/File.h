#pragma once

#include "LibLoc.h"


extern OS  os;  // Operating System Object
extern SYS sys; // Args Object

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
    File(const File& file);
    File(const xstring& i_path);
    void operator=(const File& file);

    xstring path;
    xstring data;
    xvector<File::Splits> lines;

    int non_ascii = 0;
    bool matches = false;
    bool binary = false;

    void print(const xstring& rex);
    void print_divider() const;
};

