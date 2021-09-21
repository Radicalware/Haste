#pragma once
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)

#include "Options.h"

class Core
{
    const Options& MoOptions; // based on user input

    xvector<xstring> MvsFileList;
	xvector<std::thread> MvoThreads;
    xvector<xstring> MvsFoundFiles;
    RE2 MoBackslashRex = R"((\\\\))";

public:
    Core(const Options& FoOptions);
    ~Core();

    static xstring FindMatchingFiles(xstring& FsItem, Core& FoCore);

    void MultiCoreScan();
    void SingleCoreScan();

    void PrintFiles();
};

