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
    m_rex += input.sub(R"((\\\\|/)+)", "[\\\\]+(?=[^\\\\]|$)") + ')';
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

Core::File Core::find_matching_files(xstring& item, Core& core)
{
    Core::File sp;
    if (!core.m_use_full_path)
        item = '.' + item.substr(core.m_directory.size(), item.size() - core.m_directory.size());

    sp.splits = item.inclusive_split(core.m_rex, core.m_icase, false);
    if (sp.splits.size())
        sp.matched = true;

    return sp;
}

void Core::multi_core_scan()
{
    xvector<xstring> dirs_to_scan;
    dirs_to_scan = OS::Dir(m_directory, 'd').
        xrender<xvector<xstring>>([](xstring& dir) { return OS::Dir(dir, 'r', 'f'); }).
        expand();
    dirs_to_scan += OS::Dir(m_directory, 'f');
    cout << cc::cyan << "Files in Dir: " << dirs_to_scan.size() << cc::reset << endl;

    m_file_lst = dirs_to_scan.xrender<Core::File>(Core::find_matching_files, std::ref(*this));
}

void Core::single_core_scan()
{
    xvector<xstring> dirs_to_scan = OS::Dir(m_directory, 'r', 'f');
    cout << cc::cyan << "Files in Dir: " << dirs_to_scan.size() << cc::reset << endl;

    m_file_lst = dirs_to_scan.render<Core::File>(Core::find_matching_files, std::ref(*this));
}

void Core::print_files()
{
    for (auto& file : m_file_lst)
    {
        if (!file.matched)
            continue;

        bool on  = false;

        if (file.splits[0].scan(m_rex, m_icase))
            on = true;
        

        for (auto& seg : file.splits) 
        {
            if (on) {
                cout << cc::red << cc::bold << seg << cc::reset;
                on = false;
            }
            else {
                cout << seg;
                on = true;
            }
        }
        cout << '\n';
    }
}
