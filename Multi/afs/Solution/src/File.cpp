#include "File.h"


File::File()
{
}

File::File(const File& FoFile)
{
    this->operator=(FoFile);
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
            MsData = OS::ReadFastMethod(MsPath);
        }
        catch (std::runtime_error&) {
            LbOpenFailure = true;
        }

        if (LbOpenFailure)
        {
            try {
                MsData = OS::ReadStreamMethod(MsPath);
            }
            catch (std::runtime_error& errstr) {
                this->MsData.clear();
                this->MsError = errstr.what();
            }
        }
#else
        try {
            MsData = OS::ReadStatMethod(MsPath);
        }   
        catch (std::runtime_error & errstr) {
            this->MsData.clear();
            this->MsError = errstr.what();
        }
#endif
    }
    else { // not binary searching
        try {
            MsData = OS::ReadFastMethod(MsPath);
        }
        catch (std::runtime_error& errstr) 
        {
            this->MsData.clear();
            this->MsError = errstr.what();// new xstring(errstr.what());
        }
    }
}

File::~File()
{
    //if (err != nullptr) 
    //    delete err;
}

void File::operator=(const File& FoFile)
{
    this->MsPath = FoFile.MsPath;
    this->MsData = FoFile.MsData;
    this->MvsLines = FoFile.MvsLines;
    this->MbBinary = FoFile.MbBinary;
    this->MbMatches = FoFile.MbMatches;
    this->MbBinarySearchOn = FoFile.MbBinarySearchOn;
}


void File::Print()
{
    bool LbPrinted = false;
    char spacer[3];
    if (!MbPipedData)
        strncpy(spacer, "\n\n\0", 3);
    else
        strncpy(spacer, "\0", 3);

    if (this->MbMatches && !this->MbBinary)
    {
        this->PrintDivider();
#pragma warning (suppress : 6053) // Above I enusre we get null bytes for spacer
        cout << Color::Mod::Bold << Color::Cyan << ">>> FILE: >>> " << this->MsPath.Sub(MoBackslashRex, "\\\\") << spacer << Color::Mod::Reset;
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
    cout << Color::Blue << xstring(OS::GetConsoleSize()[0], '-') << Color::Mod::Reset;
}