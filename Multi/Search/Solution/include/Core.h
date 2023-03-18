#pragma once
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)

#include "Options.h"

class Core
{
    const Options& MoOptions; // based on user input

    xvector<xp<xstring>> MvsFileList;
	xvector<std::thread> MvoThreads;
    xvector<xstring> MvsFoundFiles;
    istatic RE2 SoBackslashRex = R"(\\)";

public:
    Core(const Options& FoOptions);
    ~Core();

    static xp<xstring> FindMatchingFiles(xstring& FsItem, const Core& FoCore);

    void MultiCoreScan();
    void SingleCoreScan();

    void PrintFiles();
};

