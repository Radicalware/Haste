#pragma once

// Copyright under the Apache v2 Licence
// Created by Joel Leaugues in 2020

// -------------- STL -------------------
#include <iostream>
using std::cout;
using std::endl;
// -------------- STL -------------------
// ---------- RADICALWARE ---------------
#include "Nexus.h"
#include "xvector.h"
#include "xstring.h"
#include "OS.h"
// ---------- RADICALWARE ---------------



struct Options
{
    struct Rex
    {
        struct g2
        {
            RE2* rex = nullptr;
            re2::RE2::Options* mods = nullptr;
        };
        struct stl
        {
            std::regex rex;
            rxm::type mods;
        };

        g2 re2;
        stl std;

        xstring str;
        bool case_sensitive = false;

        ~Rex();
    };
    Options() {};
    ~Options();

    Rex rex;
    xvector<RE2*> avoid_lst;
    xstring directory;

    bool use_full_path = false;
    bool only_name_files = false;
    bool line_tracker_on = false;
    bool binary_search_on = false;
    bool entire = false;
    bool piped = false;
    bool modify = false;

    void set_dir(const xstring& input, bool use_pwd = false);
    void set_rex(const xstring& input);
    void set_avoid_regex(const xvector<xstring*>& i_avoid_lst);
};
