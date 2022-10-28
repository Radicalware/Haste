#include "File.h"


#define ReadFileCatch \
    catch (std::runtime_error& errstr) { \
        This.MsData.clear(); \
        This.MsError = errstr.what(); \
    } \
    catch (...) \
    { \
        This.MsData.clear(); \
        This.MsError = "Could Not Open/Read File"; \
    }

File::File()
{
}

File::File(const File& FoFile)
{
    This = FoFile;
}

File::File(File&& FoFile)
{
    This = std::move(FoFile);
}

File::File(const xstring& FsPath, bool FbIsBinarySearch)
{
    MsPath = FsPath;
    MbBinarySearchOn = FbIsBinarySearch;
    if (MbBinarySearchOn)
    {
#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
        bool LbOpenFailure = false;
        try {
            MsData = RA::OS::ReadFile(MsPath);
        }
        ReadFileCatch

        if (LbOpenFailure)
        {
            try {
                MsData = RA::OS::ReadStreamMethod(MsPath);
            }
            ReadFileCatch
        }
#else
        try {
            MsData = RA::OS::ReadStatMethod(MsPath);
        }   
        ReadFileCatch
#endif
    }
    else { // not binary searching
        try {
            MsData = RA::OS::ReadFile(MsPath).RemoveNonAscii();
        }
        ReadFileCatch
    }
}

void File::operator=(const File& FoFile)
{
    This.MsPath             = FoFile.MsPath;
    This.MsData             = FoFile.MsData;
    This.MvsLines           = FoFile.MvsLines;
    This.MbBinary           = FoFile.MbBinary;
    This.MbMatches          = FoFile.MbMatches;
    This.MbBinarySearchOn   = FoFile.MbBinarySearchOn;
}

void File::operator=(File&& FoFile)
{
    This.MsPath             = std::move(FoFile.MsPath);
    This.MsData             = std::move(FoFile.MsData);
    This.MvsLines           = std::move(FoFile.MvsLines);
    This.MbBinary           = FoFile.MbBinary;
    This.MbMatches          = FoFile.MbMatches;
    This.MbBinarySearchOn   = FoFile.MbBinarySearchOn;
}


void File::Print()
{
    bool LbPrinted = false;
    char spacer[3];
    if (!MbPipedData)
        strncpy(spacer, "\n\n\0", 3);
    else
        strncpy(spacer, "\0", 3);

    if (This.MbMatches && !This.MbBinary)
    {
        This.PrintDivider();
#pragma warning (suppress : 6053) // Above I enusre we get null bytes for spacer
        cout << Color::Mod::Bold << Color::Cyan << ">>> FILE: >>> " << This.MsPath.Sub(SoBackslashRex, "\\\\") << spacer << Color::Mod::Reset;
        LbPrinted = true;
    }

    // bool line_match = false; // Code review : TBR
    for (const xstring& line : MvsLines)
    {
        if (line.size())
            line.Print();
    }
    if(LbPrinted && !MbPipedData)
        cout << '\n';
}

void File::PrintDivider() const
{
    cout << Color::Blue << xstring(RA::OS::GetConsoleSize()[0], '-') << Color::Mod::Reset;
}