
#include "Options.h"


Options::Rex::~Rex()
{
    if(re2.rex) delete re2.rex;
    if(re2.mods) delete re2.mods;
}

Options::~Options()
{
    if(avoid_lst.size())
        for (RE2* val : avoid_lst)
            delete val;
}

void Options::set_dir(const xstring& input, bool use_pwd)
{
    if (input.scan(R"(\.\.[/\\])"))
        use_full_path = true;
    else if (input.match(R"(^[A-Z]\:.*$)") && !use_pwd)
        use_full_path = true;

    directory = OS::Full_Path(input);
}

void Options::set_rex(const xstring& input) 
{
#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
    rex.str = xstring('(') + input + ')';
    // swap a literal regex backslash for two literal backslashes
#else
    rex.str = rex.str + '(' + input.sub(R"(\\\\)", "\\") + ')';
#endif
    if (rex.re2.mods) delete rex.re2.mods;
    rex.re2.mods = new re2::RE2::Options;
    if (rex.case_sensitive)
    {
        rex.re2.mods->set_case_sensitive(true);
        rex.std.mods = (rxm::ECMAScript);
    }
    else {
        rex.re2.mods->set_case_sensitive(false);
        rex.std.mods = (rxm::icase | rxm::ECMAScript);
    }
    if (rex.re2.rex) delete rex.re2.rex;
    rex.re2.rex = new RE2(rex.str, *rex.re2.mods);
    rex.std.rex = std::regex(rex.str, rex.std.mods);
}

void Options::set_avoid_regex(const xvector<xstring*>& i_avoid_lst) 
{
    for (const xstring* str : i_avoid_lst)
        avoid_lst << new RE2(*str);
}

