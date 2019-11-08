#include "Core.h"
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

void Core::set_dir(const xstring& input, bool use_pwd)
{
    if (input.scan(R"(\.\.[/\\])"))
        m_use_full_path = true;
    else if (input.match(R"(^[A-Z]\:.*$)") && !use_pwd)
        m_use_full_path = true;

    m_directory = os.full_path(input);
}

void Core::set_rex(const xstring& input)
{
    m_rex = "(" + input + ")";
}

void Core::set_case_sensitive_on()
{
    m_icase = rxm::ECMAScript;
}

void Core::set_binary_on()
{
    m_binary_search = true;
}

File Core::scan_file(xstring& path, const Core& core, const OS& osi, const bool search_binary)
{
    if (!core.m_use_full_path)
        path = '.' + path(static_cast<double>(osi.pwd().size()), 0);

    File file(path);

    if (file.binary && !search_binary) 
    {
        file.data.clear();
        file.non_ascii = 1;
        return file;
    }

    if (search_binary) 
    {
        if (file.data.has_non_ascii(4))
        {
            file.matches = file.data.scan(core.m_rex, core.m_icase);
            file.data.clear();
            file.non_ascii = 1;
            return file;
        }
    }

    file.matches = file.data.scan(core.m_rex);
    if (!file.matches) {
        file.data.clear();
        return file;
    }

    unsigned long int counter = 0;
    for (const xstring& line : file.data.split('\n'))
    {
        counter++;
        // The following line actually slows it down
        //if (!line.scan(core.m_rex))
        //    continue;

        File::Splits sp;
        sp.splits = line.inclusive_split(core.m_rex, core.m_icase, false);

        if (sp.splits.size())
        {
            sp.line_num = counter;
            file.lines << sp;
        }
    }
    file.data.clear();
    return file;
}

void Core::multi_core_scan()
{

    m_file_lst = os.dir(m_directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return os.dir(dir, 'r', 'f'); }).expand();
    m_file_lst += os.dir(m_directory, 'f');
    //m_file_lst.join('\n').print();
    cout << cc::cyan << "Files in Dir: " << m_file_lst.size() << cc::reset << endl;

    // xrender is multi-threaded
    // "this" is passed in with std::ref but never modified
    m_files = m_file_lst.xrender<File>(Core::scan_file, std::ref(*this), std::ref(os), m_binary_search);
}

void Core::single_core_scan()
{
    m_file_lst = os.dir(m_directory, 'r', 'f');
    cout << cc::cyan << "Files in Dir: " << m_file_lst.size() << cc::reset << endl;

    m_files = m_file_lst.render<File>(Core::scan_file, std::ref(*this), std::ref(os), m_binary_search);
}

void Core::print()
{
    for (File& file : m_files)
    {
        if (file.non_ascii)
            continue;
        file.print(m_rex);
    };

    this->print_divider();
    bool binary_matched = false;
    for (const File& file : m_files)
    {
        if (file.non_ascii && file.matches) {
            cout << "Binary File Matches (" << file.non_ascii << "): " << file.path << '\n';
            binary_matched = true;
        }
    }
    if (binary_matched)
        this->print_divider();
}

void Core::print_divider() const
{
    cout << cc::blue << xstring(os.console_size()[0], '-') << cc::reset;
}
