#include "Core.h"
#include "OS.h"
#include "xstring.h"

#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

Core::Core(const Options& options) : MoOption(options)
{
}

Core::~Core()
{
    if (MoPipedDataPtr)
        delete MoPipedDataPtr;
    MoPipedDataPtr = nullptr; // Code review: Prevent double delete.
}


void Core::PipedScan()
{
    MoPipedDataPtr = new File;
    File& LoFile = *MoPipedDataPtr;

    LoFile.MbPipedData = true;

    LoFile.MsPath = "Piped Data";
    if (MoOption.MbEntire)
        LoFile.MbIndent = true;

    xstring LsLine = "";
    // unsigned long int LnLineNumber = 0; // Code review: Unused code. TBR.

    while (getline(std::cin, LsLine))
    {
        if (MoOption.MbEntire && LsLine.size() == 0)
        {
            LoFile.MvsLines << '\0';
            continue;
        }
        xvector<xstring> LvsSegments = LsLine.InclusiveSplit(MoOption.MoRex.MoStd.MoRex, false);
        xstring LsColoredLine = "";
        if (LvsSegments.size())
        {
            bool LbMatchOn = false;
            if (LvsSegments[0].Match(*MoOption.MoRex.MoRe2.MoRexPtr))
                LbMatchOn = true;

            for (const xstring& seg : LvsSegments)
            {
                if (LbMatchOn)
                {
                    LsColoredLine += Color::Mod::Bold + Color::Red + seg + Color::Mod::Reset;
                    LbMatchOn = false;
                }
                else {
                    LsColoredLine += seg;
                    LbMatchOn = true;
                }
            }
            LoFile.MvsLines << LsColoredLine;
        }
        else if (MoOption.MbEntire)
        {
            LoFile.MvsLines << LsLine;
        }
    }
}

void Core::ScanFile(xstring& FsPath)
{

    auto AddFile = [this](const File& FoFile)
    {
        auto Lock = MvoFiles.GetMutex().CreateLock();
        MvoFiles << FoFile;
    };

    GET(Rex, MoOption.MoRex.MoRe2.MoRexPtr);
    if (!MoOption.MbUseFullPath)
        FsPath = '.' + FsPath(static_cast<double>(OS::PWD().size()), 0);

    File LoFile(FsPath, MoOption.MbBinaraySearchOn);
    if (LoFile.MsError.size())
    {
        LoFile.MsData.clear();
        return AddFile(LoFile);
    }

    bool LbAvoid = static_cast<bool>(MoOption.MvoAvoidList.size());
    LoFile.MbMatches = LoFile.MsData.Scan(Rex);
    if (MoOption.MbModify)
        return AddFile(LoFile);

    if (!LoFile.MbMatches)
    {
        LoFile.MsData.clear();
        return AddFile(LoFile);
    }

    if (MoOption.MbBinaraySearchOn)
    {
        LoFile.MbBinary = LoFile.MsData.HasDualNulls();
        if (LoFile.MbBinary)
        {
            LoFile.MsData.clear();
            return AddFile(LoFile);
        }
    }

    xstring LsLineNumberString = "";
    xstring LsSpacer = '.';
    if (MoOption.MbOnlyNameFiles)
    {
        if (!LbAvoid)
            LoFile.MbMatches = (LoFile.MsData.Scan(Rex));
        else
            LoFile.MbMatches = (LoFile.MsData.Scan(Rex) && !LoFile.MsData.ScanList(MoOption.MvoAvoidList));
    }
    else {
        unsigned long int LnLineNumber = 0;
        for (const xstring& line : LoFile.MsData.Split('\n'))
        {
            if (LbAvoid) {
                if (line.ScanList(MoOption.MvoAvoidList))
                    continue;
            }

            LnLineNumber++;
            LsLineNumberString = RA::ToXString(LnLineNumber);
            xvector<xstring> segs = line.InclusiveSplit(MoOption.MoRex.MoStd.MoRex, false);
            xstring colored_line = "";
            if (segs.size())
            {
                colored_line = Color::Mod::Bold + Color::Cyan + "Line " + (LsSpacer * (7 - LsLineNumberString.size())) + ' ' + LsLineNumberString + ": " + Color::Mod::Reset;
                LoFile.MbMatches = true;
                bool LbMatchOn = false;
                if (segs[0].Match(Rex))
                    LbMatchOn = true;

                segs[0].LeftTrim();
                for (xstring& seg : segs)
                {
                    if (LbMatchOn)
                    {
                        colored_line += Color::Mod::Bold + Color::Red + seg + Color::Mod::Reset;
                        LbMatchOn = false;
                    }
                    else {
                        colored_line += seg;
                        LbMatchOn = true;
                    }
                }
                LoFile.MvsLines << colored_line;
            }
        }
    }
    if (!LoFile.MvsLines.size() && !MoOption.MbOnlyNameFiles) // Code review: MvsLines is not alwasys avialable 
        LoFile.MbMatches = false;
    LoFile.MsData.clear();
    AddFile(LoFile);
}

void Core::MultiCoreScan()
{
    MvsFileList = OS::Dir(MoOption.MsDirectory, 'd').ForEachThread<xvector<xstring>>([](xstring& dir) { return OS::Dir(dir, 'r', 'f'); }).Expand();
    MvsFileList += OS::Dir(MoOption.MsDirectory, 'f');

    // Filter over the avoid list.
    if (!MoOption.MvoAvoidFilesAndDirectoriesList.empty())
    {
        for (auto& x : MoOption.MvoAvoidFilesAndDirectoriesList)
            MvsFileList.erase(
                std::remove_if(MvsFileList.begin(), MvsFileList.end(),
                    [&x](xstring& element) { return element.Scan(x->pattern(), rxm::icase); }),
                MvsFileList.end());
        cout << Color::Cyan << "Files in Dir (Filtered over exclusions): " << MvsFileList.size() << Color::Mod::Reset << endl;
    }
    else
    {
        cout << Color::Cyan << "Files in Dir: " << MvsFileList.size() << Color::Mod::Reset << endl;
    }

    // xrender is multi-threaded
    // "this" is passed in with std::ref but never modified
    MvoFiles.clear();
    for (auto& File : MvsFileList)
        Nexus<>::AddJob(This, &Core::ScanFile, File);
    Nexus<>::WaitAll();
}

void Core::SingleCoreScan()
{
    MvsFileList = OS::Dir(MoOption.MsDirectory, 'r', 'f');
    // Filter over the avoid list.
    if (!MoOption.MvoAvoidFilesAndDirectoriesList.empty())
    {
        for (auto& x : MoOption.MvoAvoidFilesAndDirectoriesList)
            MvsFileList.erase(
                std::remove_if(MvsFileList.begin(), MvsFileList.end(),
                    [&x](xstring& element) { return element.Scan(x->pattern(), rxm::icase); }),
                MvsFileList.end());
        cout << Color::Cyan << "Files in Dir (Filtered over exclusions): " << MvsFileList.size() << Color::Mod::Reset << endl;
    }
    else
    {
        cout << Color::Cyan << "Files in Dir: " << MvsFileList.size() << Color::Mod::Reset << endl;
    }

    MvoFiles.clear();
    for (auto& File : MvsFileList)
        Nexus<>::AddJob(This, &Core::ScanFile, File);
    Nexus<>::WaitAll();
}

void Core::Print()
{
    if (MoPipedDataPtr != nullptr)
    {
        MoPipedDataPtr->Print();
        return;
    }

    if (MoOption.MbModify)
    {
        OS os;
        for (const File& file : MvoFiles) { // open file for modification
            if (file.MbMatches) {
                try {
                    os.RunConsoleCommand("subl " + file.MsPath);
                }
                catch (std::runtime_error& err) {
                    cout << err.what() << endl;
                }
            }
        }
    }

    if (MoOption.MbOnlyNameFiles) {   // print what the user was looking for
        this->PrintDivider();
        bool LbMatchFound = false;
        for (File& file : MvoFiles)
        {
            if (file.MbBinary || file.MsError.size())
                continue;
            if (file.MbMatches) {
                file.MsPath.Sub(MoBackslashRex, "\\\\").Print();
                LbMatchFound = true;
            }
        }
        if (!LbMatchFound)
            cout << "No Matches Found\n";
    }
    else {
        for (File& file : MvoFiles)
        {
            if (file.MbBinary || file.MsError.size())
                continue;
            file.Print();
        };
    }
    this->PrintDivider();
    bool LbBinaryMatched = false;
    for (const File& file : MvoFiles) // print binary names
    {
        if (file.MsError.size())
            continue;

        if (file.MbBinary && file.MbMatches) {
            cout << "Binary File Matches: " << file.MsPath.Sub(MoBackslashRex, "\\\\") << '\n';
            LbBinaryMatched = true;
        }
    }
    if (LbBinaryMatched)
        this->PrintDivider();

    bool LbErrorMatched = false;
    for (const File& file : MvoFiles) // print errors
    {
        if (file.MsError.size()) {
            cout << file.MsError << '\n';
            LbErrorMatched = true;
        }
    }
    if (LbErrorMatched)
        this->PrintDivider();
}

void Core::PrintDivider() const {
    cout << Color::Blue << xstring(OS::GetConsoleSize()[0], '-') << Color::Mod::Reset;
}
