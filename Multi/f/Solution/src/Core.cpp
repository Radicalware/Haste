#include "Core.h"
#pragma warning( disable : 4101 )  // for allowing the STL (non-class enum)
#pragma warning( disable : 26812 ) // for allowing the STL (non-class enum)


Core::Core(const Options& options) : m_option(options)
{
}

xstring  Core::find_matching_files(xstring& item, Core& core)
{
    if (!core.m_option.use_full_path)
        item = '.' + item.substr(core.m_option.directory.size(), item.size() - core.m_option.directory.size());

    if (core.m_option.swap_split)
#ifdef WIN_BASE
        item = item.sub("\\\\", "/");
#else
        item = item.sub("/", "\\");
#endif

    xvector<xstring> found = item.inclusive_split(core.m_option.rex.rex, false);
    xstring colored_path;
    if (!found.size())
        return colored_path;

    if (core.m_option.modify)
    {
        return item;
    }
    else if (found.size()) {
        bool match_on = false;
        if (found[0].match(core.m_option.rex.rex))
            match_on = true;


        for (const xstring& seg : found)
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
    dirs_to_scan = OS::Dir(m_option.directory, 'd').
        xrender<xvector<xstring>>([this](xstring& dir) { return OS::Dir(dir, 'r', m_option.find_mod1, m_option.find_mod2); }).
        expand();

    if(m_option.find_mod1 == 'f' || m_option.find_mod2 == 'f')
        dirs_to_scan += OS::Dir(m_option.directory, 'f');
    cout << Color::Cyan << "Files in Dir: " << dirs_to_scan.size() << Color::Mod::Reset << endl;

    // xrender is multi-threaded
    m_file_lst = dirs_to_scan.xrender(Core::find_matching_files, std::ref(*this));
}

void Core::single_core_scan()
{
    xvector<xstring> dirs_to_scan = OS::Dir(m_option.directory, 'r', m_option.find_mod1, m_option.find_mod2);
    cout << Color::Cyan << "Files in Dir: " << dirs_to_scan.size() << ' ' << Color::Mod::Reset << endl;

    // render is single-threaded
    m_file_lst = dirs_to_scan.render(Core::find_matching_files, std::ref(*this));
}

void Core::print_files()
{
    // m_file_list.join('\n').print(); // does not check for size()

    if (m_option.modify) {
        for (const xstring& file : m_file_lst)
        {
            OS os;
            if (file.size()) {
                try {
                    os.popen("subl " + file);
                }
                catch (std::runtime_error & err) {
                    cout << err.what() << endl;
                }
            }
        }
    }
    else {
        for (const xstring& file : m_file_lst)
        {
            if (file.size())
                file.print();
        }
    }
}
