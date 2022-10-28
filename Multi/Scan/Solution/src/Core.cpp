#include "Core.h"
#include "OS.h"
#include "xstring.h"

#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

void Core::Filter()
{
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

    if (!MoOption.MvoTargetFilesAndDirectoriesList.empty())
    {
        for (auto& x : MoOption.MvoTargetFilesAndDirectoriesList)
            MvsFileList.erase(
                std::remove_if(MvsFileList.begin(), MvsFileList.end(),
                    [&x](xstring& TgtFile) { return !TgtFile.Scan(x->pattern(), rxm::icase); }),
                MvsFileList.end());
        cout << Color::Cyan << "Files in Dir (Targets): " << MvsFileList.size() << Color::Mod::Reset << endl;
    }
}

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
        if (MoOption.MbEntire && LsLine.Size() == 0)
        {
            LoFile.MvsLines << '\0';
            continue;
        }
        xvector<xstring> LvsSegments = LsLine.InclusiveSplit(MoOption.MoRex.MoStd.MoRex, false);
        xstring LsColoredLine = "";
        if (LvsSegments.Size())
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
    Begin();
    auto AddFile = [this](const xp<File>& FoFilePtr) -> void
    {
        auto Lock = MvoFiles.GetMutex().CreateLock();
        MvoFiles << FoFilePtr;
    };

    GET(Rex, MoOption.MoRex.MoRe2.MoRexPtr);
    if (!MoOption.MbUseFullPath)
        FsPath = '.' + FsPath(static_cast<double>(MsPWD.Size()));

    GSS(LoFile, MKP<File>(FsPath, MoOption.MbBinaraySearchOn));
    if (LoFile.MsError.Size())
    {
        LoFile.MsData.clear();
        return AddFile(LoFilePtr);
    }

    bool LbAvoid = static_cast<bool>(MoOption.MvoAvoidList.Size());
    bool LbTarget = static_cast<bool>(MoOption.MvoTargetList.Size());
    try
    {
        LoFile.MbMatches = LoFile.MsData.Scan(Rex);
    }catch(...)
    {
        auto Lock = MvoFiles.GetMutex().CreateLock();
        cout << "Can't Scan File: " << LoFile.MsPath << '\n';
    }

    if (!LoFile.MbMatches)
    {
        LoFile.MsData.clear();
        return AddFile(LoFilePtr);
    }

    if (MoOption.MbModify)
        return AddFile(LoFilePtr);

    if (MoOption.MbBinaraySearchOn)
    {
        LoFile.MbBinary = LoFile.MsData.HasDualNulls();
        if (LoFile.MbBinary)
        {
            LoFile.MsData.clear();
            return AddFile(LoFilePtr);
        }
    }

    xstring LsLineNumberString = "";
    xstring LsSpacer = '.';
    if (MoOption.MbOnlyNameFiles)
    {
        if (LbTarget)
            LoFile.MbMatches = (LoFile.MbMatches && LoFile.MsData.ScanList(MoOption.MvoTargetList));
        
        if (LbAvoid && !(LbTarget && LoFile.MbMatches))
            LoFile.MbMatches = (LoFile.MbMatches && !LoFile.MsData.ScanList(MoOption.MvoAvoidList));
    }
    else {
        unsigned long int LnLineNumber = 0;
        for (const xstring& line : LoFile.MsData.Split('\n'))
        {
            if (LbTarget) {
                if (!line.ScanList(MoOption.MvoTargetList))
                    continue;
            }

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
                colored_line = Color::Mod::Bold + Color::Cyan + "Line " + (LsSpacer * (7 - LsLineNumberString.Size())) + ' ' + LsLineNumberString + ": " + Color::Mod::Reset;
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
    if (!LoFile.MvsLines.Size() && !MoOption.MbOnlyNameFiles) // Code review: MvsLines is not alwasys avialable 
        LoFile.MbMatches = false;
    LoFile.MsData.clear();
    AddFile(LoFilePtr);
    Rescue();
}

void Core::MultiCoreScan()
{
    Begin();
    MvsFileList = 
        RA::OS::Dir(MoOption.MsDirectory, 'd')
        .ForEachThread<xvector<xstring>>([](xstring& dir) { return RA::OS::Dir(dir, 'r', 'f'); })
        .Expand();
    MvsFileList += RA::OS::Dir(MoOption.MsDirectory, 'f');


    Filter();
    cout << Color::Cyan << "Files in Dir: " << MvsFileList.size() << Color::Mod::Reset << endl;
    
    // xrender is multi-threaded
    // "this" is passed in with std::ref but never modified
    MvoFiles.clear();
    for (auto& File : MvsFileList)
        Nexus<>::AddJob(This, &Core::ScanFile, File);
    Nexus<>::WaitAll();
    Rescue();
}

void Core::SingleCoreScan()
{
    Begin();
    MvsFileList = RA::OS::Dir(MoOption.MsDirectory, 'r', 'f');

    Filter();
    cout << Color::Cyan << "Files in Dir: " << MvsFileList.size() << Color::Mod::Reset << endl;

    MvoFiles.clear();
    for (auto& File : MvsFileList)
        This.ScanFile(File);
    Nexus<>::WaitAll();
    Rescue();
}

void Core::Print()
{
    Begin();
    if (MoPipedDataPtr != nullptr)
    {
        MoPipedDataPtr->Print();
        return;
    }

    if (MoOption.MbModify)
    {
        RA::OS OS;
        for (const xp<File> TargetPtr : MvoFiles) // open file for modification
        { 
            GSS(Target);
            if (Target.MbMatches) 
            {
                try {
                    OS.RunConsoleCommand("subl " + Target.MsPath);
                }
                catch (std::runtime_error& err) {
                    cout << err.what() << endl;
                }
            }
        }
    }

    if (MoOption.MbOnlyNameFiles) // print what the user was looking for
    {
        This.PrintDivider();
        bool LbMatchFound = false;
        for (xp<File>& TargetPtr : MvoFiles)
        {
            GSS(Target);
            if (Target.MbBinary || Target.MsError.size())
                continue;
            if (Target.MbMatches) {
                Target.MsPath.Sub(MoBackslashRex, "\\\\").Print();
                LbMatchFound = true;
            }
        }
        if (!LbMatchFound)
            cout << "No Matches Found\n";
    }
    else {
        for (xp<File>& TargetPtr : MvoFiles)
        {
            GSS(Target);
            if (Target.MbBinary || Target.MsError.size())
                continue;
            Target.Print();
        };
    }

    This.PrintDivider();
    bool LbBinaryMatched = false;
    for (const xp<File>& TargetPtr : MvoFiles) // print binary names
    {
        GSS(Target);
        if (Target.MsError.Size())
            continue;

        if (Target.MbBinary && Target.MbMatches) {
            cout << "Binary File Matches: " << Target.MsPath.Sub(MoBackslashRex, "\\\\") << '\n';
            LbBinaryMatched = true;
        }
    }
    if (LbBinaryMatched)
        This.PrintDivider();

    bool LbErrorMatched = false;
    for (const xp<File>& TargetPtr : MvoFiles) // print errors
    {
        GSS(Target);
        if (Target.MsError.Size()) {
            cout << Target.MsError << '\n';
            LbErrorMatched = true;
        }
    }
    if (LbErrorMatched)
        This.PrintDivider();
    Rescue();
}

void Core::PrintDivider() const {
    cout << Color::Blue << xstring(RA::OS::GetConsoleSize()[0], '-') << Color::Mod::Reset;
}
