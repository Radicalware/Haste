#pragma once
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

#include "File.h"
#include "Options.h"

class Core
{
    const Options& MoOption; // based on user input

    xvector<File> MvoFiles; // list of file data (split lines)
    xvector<xstring> MvsFileList; // list of file names in the m_directory

    File* MoPipedDataPtr = nullptr;
    const RE2 MoBackslashRex = R"(\\)";

public:
    Core(const Options& FoOptions);
    ~Core();

    void PipedScan();
    static File ScanFile(xstring& FsPath, const Core& FoCore, const bool FbSearchBinary);

    void MultiCoreScan();
    void SingleCoreScan();

    void Print();
    void PrintDivider() const;
};

