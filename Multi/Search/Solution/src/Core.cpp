#include "Core.h"
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)


Core::Core(const Options& FoOptions) : MoOptions(FoOptions)
{
}

Core::~Core()
{
}

xp<xstring>  Core::FindMatchingFiles(xstring& FsItem, const Core& FoCore)
{
    Begin();
    // note: at this point, there are only single backslashes
    // the inclusive-split sets them to double backslash to cancel out special character
    bool LbMatched = false;
    if (FoCore.MoOptions.MvoAvoidList.size())
        LbMatched = (FsItem.Scan(*FoCore.MoOptions.MoRegularExpression.MoRegularExpressionG2.MoRegularExpressionPtr) 
            && !FsItem.ScanList(FoCore.MoOptions.MvoAvoidList));
    else
        LbMatched = (FsItem.Scan(*FoCore.MoOptions.MoRegularExpression.MoRegularExpressionG2.MoRegularExpressionPtr));

    if (!LbMatched)
        return xstring();

    if (!FoCore.MoOptions.MbUseFullPath)
        FsItem = "." + FsItem.substr(FoCore.MoOptions.MsDirectory.size(), FsItem.size() - FoCore.MoOptions.MsDirectory.size());
    
    if (FoCore.MoOptions.MbSwapSplit)
#ifdef WIN_BASE
        FsItem.InSub('\\', "/");
#else
        FsItem.InSub('/', "\\");
#endif
    xvector<xstring> LvsFound = FsItem.InclusiveSplit(FoCore.MoOptions.MoRegularExpression.MoStd.MoRegularExpression, false);
    GET(LsColoredPath, MKP<xstring>());
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
    return LsColoredPathPtr;
    Rescue();
}

void Core::MultiCoreScan()
{
    Begin();
    xvector<xp<xstring>> LvsDirectoriesToScan;
    LvsDirectoriesToScan = RA::OS::Dir(MoOptions.MsDirectory, 'd').
        ForEachThread<xvector<xstring>>([this](xstring& dir) { return RA::OS::Dir(dir, 'r', MoOptions.McFindMod1, MoOptions.McFindMod2); }).
        Expand();

    if(MoOptions.McFindMod1 == 'f' || MoOptions.McFindMod2 == 'f')
        LvsDirectoriesToScan += RA::OS::Dir(MoOptions.MsDirectory, 'f');
    cout << Color::Cyan << "Files in Dir: " << LvsDirectoriesToScan.size() << Color::Mod::Reset << endl;

    // xrender is multi-threaded
   MvsFileList = 
        LvsDirectoriesToScan.ForEachThread<xstring>(&Core::FindMatchingFiles, The);
    Rescue();
}

void Core::SingleCoreScan()
{
    Begin();
    xvector<xstring> LvsDirectoriesToScan = RA::OS::Dir(MoOptions.MsDirectory, 'r', MoOptions.McFindMod1, MoOptions.McFindMod2);
    cout << Color::Cyan << "Files in Dir: " << LvsDirectoriesToScan.size() << ' ' << Color::Mod::Reset << endl;

    // render is single-threaded
    MvsFileList = LvsDirectoriesToScan.ForEach<xp<xstring>>(&Core::FindMatchingFiles, The);
    Rescue();
}

void Core::PrintFiles()
{
    Begin();
    if (MoOptions.MbModify) {
        for (const xp<xstring>& FsFileNamePtr : MvsFileList)
        {
            GET(FsFileName);
            RA::OS OS;
            if (FsFileName.Size()) {
                try {
                    OS.RunConsoleCommand("subl " + FsFileName);
                }
                catch (std::runtime_error & err) {
                    printf(err.what());
                }
            }
        }
    }
    else 
        for (const xp<xstring>& FsFileNamePtr : MvsFileList) 
            if (FsFileNamePtr.Get().Size()) 
                FsFileNamePtr.Get().Print();
    Rescue();
}
