
#include "Options.h"


Options::~Options()
{
    for (auto* item : avoid_lst)
        delete item;
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
    rex.re2.rex = new RE2(rex.str, *rex.re2.mods);
    rex.std.rex = std::regex(rex.str, rex.std.mods);
}

void Options::set_avoid_regex(const xvector<xstring*>& i_avoid_lst)
{
    for (const xstring* str : i_avoid_lst)
        avoid_lst << new re2::RE2(*str, *rex.re2.mods);
}

void Options::return_only(const xstring& ret_only)
{
    if (ret_only == "d" || ret_only == "dirs") {
        find_mod1 = 'd';
        find_mod2 = 'd';
    }
    else if (ret_only == "b" || ret_only == "both") {
        find_mod1 = 'd';
        find_mod2 = 'f';
    }
    //else if(ret_type == "files") // default
}

Options::Rex::~Rex()
{
    delete re2.rex;
    delete re2.mods;
}
