#include "Options.h"

Options::~Options()
{
}

void Options::SetDirectory(const xstring& FsInput, bool FbUsePassword)
{
    Begin();
    if (FsInput.Scan(R"(\.\.[/\\])"))
        MbUseFullPath = true;
    else if (FsInput.Match(R"(^[A-Z]\:.*$)") && !FbUsePassword)
        MbUseFullPath = true;

    MsDirectory = RA::OS::FullPath(FsInput);
    Rescue();
}

void Options::SetRegex(const xstring& FsInput)
{
    Begin();
#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
    MoRegularExpression.MsString = xstring('(') + FsInput + ')';
    // swap a literal regex backslash for two literal backslashes
#else
    rex.str = rex.str + '(' + input.Sub(R"(\\\\)", "\\") + ')';
#endif
    MoRegularExpression.MoRegularExpressionG2.MoModsPtr = MKP<re2::RE2::Options>();
    if (MoRegularExpression.MbCaseSensitive)
    {
        MoRegularExpression.MoRegularExpressionG2.MoModsPtr->set_case_sensitive(true);
        MoRegularExpression.MoStd.MoMods = (RXM::ECMAScript);
    }
    else {
        MoRegularExpression.MoRegularExpressionG2.MoModsPtr->set_case_sensitive(false);
        MoRegularExpression.MoStd.MoMods = (RXM::icase | RXM::ECMAScript);
    }
    MoRegularExpression.MoRegularExpressionG2.MoRegularExpressionPtr = MKP<RE2>(MoRegularExpression.MsString, *MoRegularExpression.MoRegularExpressionG2.MoModsPtr);
    MoRegularExpression.MoStd.MoRegularExpression = std::regex(MoRegularExpression.MsString, MoRegularExpression.MoStd.MoMods);
    Rescue();
}

void Options::SetAvoidRegex(const xvector<xstring>& FvsAvoidList)
{
    Begin();
    for (const xstring& str : FvsAvoidList)
        MvoAvoidList << MKP<re2::RE2>(str, *MoRegularExpression.MoRegularExpressionG2.MoModsPtr);
    Rescue();
}

void Options::SetReturnOnly(const xstring& FxReturnOnly)
{
    if (FxReturnOnly == "d" || FxReturnOnly == "dirs") {
        McFindMod1 = 'd';
        McFindMod2 = 'd';
    }
    else if (FxReturnOnly == "b" || FxReturnOnly == "both") {
        McFindMod1 = 'd';
        McFindMod2 = 'f';
    }
    //else if(ret_type == "files") // default
}

Options::Rex::~Rex()
{
}
