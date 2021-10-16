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
            RE2* MoRexPtr = nullptr;
            re2::RE2::Options* MoModsPtr = nullptr;
        };
        struct stl
        {
            std::regex MoRex;
            rxm::type MoMods;
        };

        g2 MoRe2;
        stl MoStd;

        xstring MsStr = "";
        bool MbCaseSensitive = false;

        Rex() {}
        ~Rex();
    };
    Options() {};
    ~Options();

    Rex MoRex;
    xvector<RE2*> MvoAvoidList;
    xvector<RE2*> MvoAvoidFilesAndDirectoriesList;
    xstring MsDirectory = "";

    bool MbUseFullPath = false;
    bool MbOnlyNameFiles = false;
    bool MbLineTrackerOn = false;
    bool MbBinaraySearchOn = false;
    bool MbEntire = false;
    bool MbPiped = false;
    bool MbModify = false;

    void SetDirectory(const xstring& FsInput, bool FbUsePassword = false);
    void SetRegex(const xstring& FsInput);
    void SetAvoidRegex(const xvector<xstring>& FvsAvoidList);
    void SetAvoidDirectories(const xvector<xstring>& FvsAvoidList);
};