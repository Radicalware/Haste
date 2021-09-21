#include "Core.h"
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)


Core::Core(const Options& FoOptions) : MoOptions(FoOptions)
{
}

Core::~Core()
{
}

xstring  Core::FindMatchingFiles(xstring& FsItem, Core& FoCore)
{
    // note: at this point, there are only single backslashes
    // the inclusive-split sets them to double backslash to cancel out special characters.


    bool LbMatched = false;
    if (FoCore.MoOptions.MvoAvoidList.size())
        LbMatched = (FsItem.Scan(*FoCore.MoOptions.MoRegularExpression.MoRegularExpressionG2.MoRegularExpressionPtr) && !FsItem.ScanList(FoCore.MoOptions.MvoAvoidList));
    else
        LbMatched = (FsItem.Scan(*FoCore.MoOptions.MoRegularExpression.MoRegularExpressionG2.MoRegularExpressionPtr));

    if (!LbMatched)
        return xstring();

    if (!FoCore.MoOptions.MbUseFullPath)
        FsItem = "." + FsItem.substr(FoCore.MoOptions.MsDirectory.size(), FsItem.size() - FoCore.MoOptions.MsDirectory.size());
    
    
    if (FoCore.MoOptions.MbSwapSplit)
#ifdef WIN_BASE
        FsItem = FsItem.Sub("\\\\", "/");
#else
        FsItem = FsItem.Sub("/", "\\");
#endif
    xvector<xstring> LvsFound = FsItem.InclusiveSplit(FoCore.MoOptions.MoRegularExpression.MoStd.MoRegularExpression, false);
    xstring LsColoredPath;
    if (!LvsFound.size())
        return LsColoredPath;

    if (FoCore.MoOptions.MbModify)
    {
        return FsItem;
    }
    else if (LvsFound.size()) 
    {
        bool LbMatchOn = false;
        if (LvsFound[0].Match(*FoCore.MoOptions.MoRegularExpression.MoRegularExpressionG2.MoRegularExpressionPtr))
            LbMatchOn = true;


        for (xstring& seg : LvsFound)
        {
            if (LbMatchOn)
            {
                LsColoredPath += Color::Mod::Bold + Color::Red + seg + Color::Mod::Reset;
                LbMatchOn = false;
            }
            else {
                LsColoredPath += seg;
                LbMatchOn = true;
            }
        }
    }
    return LsColoredPath;
}

void Core::MultiCoreScan()
{
    xvector<xstring> LvsDirectoriesToScan;
    LvsDirectoriesToScan = OS::Dir(MoOptions.MsDirectory, 'd').
        ForEachThread<xvector<xstring>>([this](xstring& dir) { return OS::Dir(dir, 'r', MoOptions.McFindMod1, MoOptions.McFindMod2); }).
        Expand();

    if(MoOptions.McFindMod1 == 'f' || MoOptions.McFindMod2 == 'f')
        LvsDirectoriesToScan += OS::Dir(MoOptions.MsDirectory, 'f');
    cout << Color::Cyan << "Files in Dir: " << LvsDirectoriesToScan.size() << Color::Mod::Reset << endl;

    // xrender is multi-threaded
    MvsFileList = LvsDirectoriesToScan.ForEachThread(Core::FindMatchingFiles, std::ref(*this));
}

void Core::SingleCoreScan()
{
    xvector<xstring> LvsDirectoriesToScan = OS::Dir(MoOptions.MsDirectory, 'r', MoOptions.McFindMod1, MoOptions.McFindMod2);
    cout << Color::Cyan << "Files in Dir: " << LvsDirectoriesToScan.size() << ' ' << Color::Mod::Reset << endl;

    // render is single-threaded
    MvsFileList = LvsDirectoriesToScan.ForEach(Core::FindMatchingFiles, std::ref(*this));
}

void Core::PrintFiles()
{
    // m_file_list.Join('\n').Print(); // does not check for size()

    if (MoOptions.MbModify) {
        for (const xstring& file : MvsFileList)
        {
            OS os;
            if (file.size()) {
                try {
                    os.RunConsoleCommand("subl " + file);
                }
                catch (std::runtime_error & err) {
                    cout << err.what() << endl;
                }
            }
        }
    }
    else for (const xstring& file : MvsFileList) if (file.size()) file.Print();
    // bad syntax but it looks cool
}
