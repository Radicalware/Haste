#include "Core.h"
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)

void Core::set_case_sensitive()
{
    m_icase = rxm::ECMAScript;
}

void Core::set_dir(const xstring& input, bool use_pwd)
{
	if (input.scan(R"(\.\.[/\\])"))
		m_use_full_path = true;
	else if (input.match(R"(^[A-Z]\:.*$)") && !use_pwd)
		m_use_full_path = true;

	m_directory = OS::Full_Path(input);
}

void Core::set_rex(const xstring& input)
{
#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
    //m_rex += input.sub(R"((\\\\|/)+)", "[\\\\]+(?=[^\\\\]|$)") + ')';
    m_rex = input;
#else
    m_rex += input.sub(R"(\\\\)", "\\" ) + ')';
#endif
}

void Core::set_full_path(){
    m_use_full_path = true;
}

void Core::swap_split()
{
    m_swap_split = true;
}

void Core::return_only(const xstring& ret_only)
{
    if (ret_only == "d" || ret_only == "dirs") {
        m_find_mod1 = 'd';
        m_find_mod2 = 'd';
    }
    else if (ret_only == "b" || ret_only == "both") {
        m_find_mod1 = 'd';
        m_find_mod2 = 'f';
    }
    //else if(ret_type == "files") // default
}

xstring  Core::find_matching_files(xstring& item, Core& core)
{
    if (!core.m_use_full_path)
        item = '.' + item.substr(core.m_directory.size(), item.size() - core.m_directory.size());

    if (core.m_swap_split)
#ifdef WIN_BASE
        item = item.sub("\\\\", "/");
#else
        item = item.sub("/", "\\");
#endif

    xvector<xstring> found = item.inclusive_split(core.m_rex, core.m_icase, false);
    xstring colored_path;
    if (found.size())
    {
        bool match_on = false;
        if (found[0].match(core.m_rex, core.m_icase))
            match_on = true;
              

        for(const xstring& seg: found)
        {
            if (match_on)
            {
                colored_path += Color::Mod::Bold + Color::Red + seg + Color::Mod::Reset;
                match_on = false;
            }
            else {
                colored_path += seg;
                match_on = true;
            }
        }
    }
    return colored_path;
}

void Core::multi_core_scan()
{
    xvector<xstring> dirs_to_scan;
    dirs_to_scan = OS::Dir(m_directory, 'd').
        xrender<xvector<xstring>>([this](xstring& dir) { return OS::Dir(dir, 'r', m_find_mod1, m_find_mod2); }).
        expand();

    if(m_find_mod1 == 'f' || m_find_mod2 == 'f')
        dirs_to_scan += OS::Dir(m_directory, 'f');
    cout << cc::cyan << "Files in Dir: " << dirs_to_scan.size() << cc::reset << endl;

    // xrender is multi-threaded
    m_file_lst = dirs_to_scan.xrender(Core::find_matching_files, std::ref(*this));
}

void Core::single_core_scan()
{
    xvector<xstring> dirs_to_scan = OS::Dir(m_directory, 'r', m_find_mod1, m_find_mod2);
    cout << cc::cyan << "Files in Dir: " << dirs_to_scan.size() << cc::reset << endl;

    // render is single-threaded
    m_file_lst = dirs_to_scan.render(Core::find_matching_files, std::ref(*this));
}

void Core::print_files()
{
    // m_file_list.join('\n').print(); // does not check for size()
    for (const xstring& file : m_file_lst)
    {
        if (file.size()) 
            file.print();
    }
}
