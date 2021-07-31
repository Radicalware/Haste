
#include "Options.h"


Options::~Options()
{
    for (auto* item : MvoAvoidList)
        delete item;
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
    MoRegularExpression.MsString = xstring('(') + FsInput + ')';
    // swap a literal regex backslash for two literal backslashes
#else
    rex.str = rex.str + '(' + input.Sub(R"(\\\\)", "\\") + ')';
#endif
    MoRegularExpression.MoRegularExpressionG2.MoModsPtr = new re2::RE2::Options;
    if (MoRegularExpression.MbCaseSensitive)
    {
        MoRegularExpression.MoRegularExpressionG2.MoModsPtr->set_case_sensitive(true);
        MoRegularExpression.MoStd.MoMods = (rxm::ECMAScript);
    }
    else {
        MoRegularExpression.MoRegularExpressionG2.MoModsPtr->set_case_sensitive(false);
        MoRegularExpression.MoStd.MoMods = (rxm::icase | rxm::ECMAScript);
    }
    MoRegularExpression.MoRegularExpressionG2.MoRegularExpressionPtr = new RE2(MoRegularExpression.MsString, *MoRegularExpression.MoRegularExpressionG2.MoModsPtr);
    MoRegularExpression.MoStd.MoRegularExpression = std::regex(MoRegularExpression.MsString, MoRegularExpression.MoStd.MoMods);
}

void Options::SetAvoidRegex(const xvector<xstring*>& FvsAvoidList)
{
    for (const xstring* str : FvsAvoidList)
        MvoAvoidList << new re2::RE2(*str, *MoRegularExpression.MoRegularExpressionG2.MoModsPtr);
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
    delete MoRegularExpressionG2.MoRegularExpressionPtr;
    delete MoRegularExpressionG2.MoModsPtr;
}
