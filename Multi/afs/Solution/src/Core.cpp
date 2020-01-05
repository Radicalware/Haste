#include "Core.h"
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

Core::Core(const Options& options): m_option(options)
{
}

Core::~Core()
{
    if (m_piped_data != nullptr)
        delete m_piped_data;
}


void Core::piped_scan()
{
    m_piped_data = new File;
    File& file = *m_piped_data;

    file.piped_data = true;

    file.path = "Piped Data";
    if(m_option.entire)
        file.indent = true;

    xstring line;
    unsigned long int line_num = 0;

    while (getline(std::cin, line))
    {
        xvector<xstring> segs = line.inclusive_split(m_option.rex.rex, false);
        xstring colored_line;
        if (segs.size())
        {
            bool match_on = false;
            if (segs[0].match(m_option.rex.rex))
                match_on = true;

            for (const xstring& seg : segs)
            {
                if (match_on)
                {
                    colored_line += Color::Mod::Bold + Color::Red + seg + Color::Mod::Reset;
                    match_on = false;
                }
                else {
                    colored_line += seg;
                    match_on = true;
                }
            }
            file.lines << colored_line;
        }
        else if (m_option.entire)
        {
            file.lines << line;
        }
    }
}

File Core::scan_file(xstring& path, const Core& core, const bool binary_search_on)
{
    if (!core.m_option.use_full_path)
        path = '.' + path(static_cast<double>(OS::PWD().size()), 0);

    File file(path, binary_search_on);
    if (file.err.size())
    {
        file.data.clear();
        return file;
    }

    bool avoid = static_cast<bool>(core.m_option.avoid_lst.size());
    file.matches = file.data.scan(core.m_option.rex.rex);
    if (core.m_option.modify)
        return file;

    if (!file.matches)
    {
        file.data.clear();
        return file;
    }

    if (binary_search_on)
    {
        file.binary = file.data.has_dual_nulls(); 
        if (file.binary)
        {
            file.data.clear();
            return file;
        }
    }

    xstring line_num_str;
    xstring spacer = '.';
    if (core.m_option.only_name_files)
    {
        if (!avoid)
            file.matches = (file.data.scan(core.m_option.rex.rex));
        else
            file.matches = (file.data.scan(core.m_option.rex.rex) && !file.data.scan_list(core.m_option.avoid_lst));
    }
    else {
        unsigned long int line_num = 0;
        for (const xstring& line : file.data.split('\n'))
        {
            if (avoid) {
                if (line.scan_list(core.m_option.avoid_lst))
                    continue;
            }

            line_num++;
            line_num_str = to_xstring(line_num);
            xvector<xstring> segs = line.inclusive_split(core.m_option.rex.rex, false);
            xstring colored_line;
            if (segs.size())
            {
                colored_line = Color::Mod::Bold + Color::Cyan + "Line " + (spacer * (7 - line_num_str.size())) + ' ' + line_num_str + ": " + Color::Mod::Reset;
                file.matches = true;
                bool match_on = false;
                if (segs[0].match(core.m_option.rex.rex))
                    match_on = true;
                
                segs[0].ltrim();
                for (xstring& seg : segs)
                {
                    if (match_on)
                    {
                        colored_line += Color::Mod::Bold + Color::Red + seg + Color::Mod::Reset;
                        match_on = false;
                    }
                    else {
                        colored_line += seg;
                        match_on = true;
                    }
                }
                file.lines << colored_line;
            }
        }
    }
    file.data.clear();
    return file;
}

void Core::multi_core_scan()
{
    m_file_lst = OS::Dir(m_option.directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return OS::Dir(dir, 'r', 'f'); }).expand();
    m_file_lst += OS::Dir(m_option.directory, 'f');

    cout << Color::Cyan << "Files in Dir: " << m_file_lst.size() << Color::Mod::Reset << endl;

    // xrender is multi-threaded
    // "this" is passed in with std::ref but never modified
    m_files = m_file_lst.xrender<File>(Core::scan_file, std::ref(*this), m_option.binary_search_on);
}

void Core::single_core_scan()
{
    m_file_lst = OS::Dir(m_option.directory, 'r', 'f');
    cout << Color::Cyan << "Files in Dir: " << m_file_lst.size() << Color::Mod::Reset << endl;

    m_files = m_file_lst.render<File>(Core::scan_file, std::ref(*this), m_option.binary_search_on);
}

void Core::print()
{
    if (m_piped_data != nullptr)
    {
        m_piped_data->print();
        return;
    }

    if (m_option.modify)
    {
        OS os;
        for (const File& file : m_files) {
            if (file.matches) {
                try {
                    os.popen("subl " + file.path);
                }
                catch (std::runtime_error & err) {
                    cout << err.what() << endl;
                }
            }
        }
    }

    if (m_option.only_name_files) {   // print what the user was looking for
        this->print_divider();
        bool match_found = false;
        for (File& file : m_files)
        {
            if (file.binary || file.err.size())
                continue;
            if (file.matches) {
                file.path.print();
                match_found = true;
            }
        }
        if (!match_found)
            cout << "No Matches Found\n";
    }
    else {
        for (File& file : m_files)
        {
            if (file.binary || file.err.size())
                continue;
            file.print();
        };
    }
    this->print_divider();
    bool binary_matched = false;
    for (const File& file : m_files) // print binary names
    {
        if (file.err.size())
            continue;

        if (file.binary && file.matches) {
            cout << "Binary File Matches: " << file.path << '\n';
            binary_matched = true;
        }
    }
    if (binary_matched)
        this->print_divider();

    bool err_matched = false;
    for (const File& file : m_files) // print errors
    {
        if (file.err.size()) {
            cout << file.err << '\n';
            err_matched = true;
        }
    }
    if (err_matched)
        this->print_divider();
}

void Core::print_divider() const {
    cout << Color::Blue << xstring(OS::Console_Size()[0], '-') << Color::Mod::Reset;
}
