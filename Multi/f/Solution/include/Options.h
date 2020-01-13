#pragma once

// Copyright under the Apache v2 Licence
// Created by Joel Leaugues in 2020

// -------------- STL -------------------
#include <iostream>
#include <regex>
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
            RE2* rex;
            re2::RE2::Options* mods;
        };
        struct stl
        {
            std::regex rex;
            #pragma warning (suppress : 26812)
            rxm::type mods = (rxm::icase | rxm::ECMAScript);
        };
        g2 re2;
        stl std;
        xstring str;

        bool case_sensitive = false;
        
        ~Rex();
    };

    ~Options();

    Rex rex;
    xvector<re2::RE2*> avoid_lst;
    xstring directory;

    bool use_full_path = false;
    bool swap_split = false;
    bool modify = false;

    char find_mod1 = 'f';
    char find_mod2 = 'f';

    void set_dir(const xstring& input, bool use_pwd = false);
    void set_rex(const xstring& input);
    void set_avoid_regex(const xvector<xstring*>& i_avoid_lst);
    void return_only(const xstring& ret_only);
};
