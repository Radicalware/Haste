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
            RE2* MoRegularExpressionPtr = nullptr;
            re2::RE2::Options* MoModsPtr = nullptr;
        };
        struct stl
        {
            std::regex MoRegularExpression;
            #pragma warning (suppress : 26812)
            rxm::type MoMods = (rxm::icase | rxm::ECMAScript);
        };
        g2 MoRegularExpressionG2;
        stl MoStd;
        xstring MsString;

        bool MbCaseSensitive = false;
        
        ~Rex();
    };

    ~Options();

    Rex MoRegularExpression;
    xvector<re2::RE2*> MvoAvoidList;
    xstring MsDirectory;

    bool MbUseFullPath = false;
    bool MbSwapSplit = false;
    bool MbModify = false;

    char McFindMod1 = 'f';
    char McFindMod2 = 'f';

    void SetDirectory(const xstring& FsInput, bool FbUsePassword = false);
    void SetRegex(const xstring& FsInput);
    void SetAvoidRegex(const xvector<xstring*>& FvsAvoidList);
    void SetReturnOnly(const xstring& FxReturnOnly);
};
