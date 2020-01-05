
#include "Options.h"


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
    rex.str.clear();
#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
    //m_rex += input.sub(R"((\\\\|/)+)", "[\\\\]+(?=[^\\\\]|$)") + ')';
    rex.str = rex.str + '(' + input + ')';
#else
    rex.str = rex.str + '(' + input.sub(R"(\\\\)", "\\") + ')';
#endif
    rex.rex = std::regex(rex.str, rex.mods);
}

void Options::set_avoid_regex(const xvector<xstring*>& i_avoid_lst)
{
    for (const xstring* str : i_avoid_lst)
        avoid_lst << std::regex(*str, rex.mods);
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
