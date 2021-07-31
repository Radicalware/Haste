#pragma once

#include "OS.h"

#include "xvector.h"
#include "xstring.h"

#include <iostream>
using std::cout;
using std::endl;

struct File
{
    const RE2 MoBackslashRex = R"((\\\\))";

    xstring MsPath = "";
    xstring MsData = "";
    xvector<xstring> MvsLines;
    xstring MsError = "";

    bool MbPipedData = false;
    bool MbBinary = false;
    bool MbMatches = false;
    bool MbBinarySearchOn = false;
    bool MbIndent = false;

    File();
    ~File();
    File(const File& FoFile);
    File(const xstring& FsPath, bool FbIsBinarySearch);
    void operator=(const File& FoFile);

    void Print();
    void PrintDivider() const;
};
