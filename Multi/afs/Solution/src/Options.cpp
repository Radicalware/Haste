
#include "Options.h"


Options::Rex::~Rex()
{
    if(MoRe2.MoRexPtr) delete MoRe2.MoRexPtr;
    if(MoRe2.MoModsPtr) delete MoRe2.MoModsPtr;
}

Options::~Options()
{
    if(MvoAvoidList.size())
        for (RE2* val : MvoAvoidList)
            delete val;
    if(!MvoAvoidFilesAndDirectoriesList.empty())
        for (RE2* val : MvoAvoidFilesAndDirectoriesList)
            delete val;
    // Make sure we don't delete the elements twice
    MvoAvoidList.clear();
    MvoAvoidFilesAndDirectoriesList.clear();
}

void Options::SetDirectory(const xstring& FsInput, bool FbUsePassword)
{
    if (FsInput.Scan(R"(\.\.[/\\])"))
        MbUseFullPath = true;
    else if (FsInput.Match(R"(^[A-Z]\:.*$)") && !FbUsePassword)
        MbUseFullPath = true;

    MsDirectory = OS::FullPath(FsInput);
}

void Options::SetRegex(const xstring& FsInput) 
{
#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
    MoRex.MsStr = xstring('(') + FsInput + ')';
    // swap a literal regex backslash for two literal backslashes
#else
    MoRex.MsStr = MoRex.MsStr + '(' + FsInput.Sub(R"(\\\\)", "\\") + ')';
#endif
    if (MoRex.MoRe2.MoModsPtr) delete MoRex.MoRe2.MoModsPtr;
    MoRex.MoRe2.MoModsPtr = new re2::RE2::Options;
    if (MoRex.MbCaseSensitive)
    {
        MoRex.MoRe2.MoModsPtr->set_case_sensitive(true);
        MoRex.MoStd.MoMods = (rxm::ECMAScript);
    }
    else {
        MoRex.MoRe2.MoModsPtr->set_case_sensitive(false);
        MoRex.MoStd.MoMods = (rxm::icase | rxm::ECMAScript);
    }
    if (MoRex.MoRe2.MoRexPtr) delete MoRex.MoRe2.MoRexPtr;
    MoRex.MoRe2.MoRexPtr = new RE2(MoRex.MsStr, *MoRex.MoRe2.MoModsPtr);
    MoRex.MoStd.MoRex = std::regex(MoRex.MsStr, MoRex.MoStd.MoMods);
}

void Options::SetAvoidRegex(const xvector<xstring*>& FvsAvoidList) 
{
    for (const xstring* str : FvsAvoidList)
        MvoAvoidList << new RE2(*str);
}

void Options::SetAvoidDirectories(const xvector<xstring*>& FvsAvoidList)
{
    for (const xstring* str : FvsAvoidList)
        MvoAvoidFilesAndDirectoriesList << new RE2(*str);
}
