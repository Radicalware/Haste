#pragma once

#include "LibLoc.h"

struct File
{
    struct Splits // Splits up each line of a file
    {
        xvector<xstring> rex_on;
        xvector<xstring> rex_off;
        unsigned long long int line_num = 0;    
        Splits();
        Splits(const Splits& split);
    };

    File();
    File(const File& file);
    File(const xstring& i_path);
    void operator=(const File& file);

    xstring path;
    xstring data;
    xvector<File::Splits> lines;

    bool non_ascii = false;
    bool matches = false;

    void print(bool* line_tracker_on, bool show_non_matches) const;
    void print_divider() const;
};
