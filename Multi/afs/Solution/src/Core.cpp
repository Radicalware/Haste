#include "Core.h"
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

void Core::parse_as_regex(bool input)
{
    m_parse_as_regex = input;
}

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

    m_directory = os.full_path(input);
}

void Core::set_rex(const xstring& input)
{
    m_rex = '(' + input + ')';
}

void Core::gather_files()
{
    m_file_lst = os.dir(m_directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return os.dir(dir, 'r', 'f'); }).expand();
    m_file_lst += os.dir(m_directory, 'f');
    cout << "Files in Dir: " << m_file_lst.size() << endl;
    //m_file_lst.join('\n').print();
}

void Core::find_matching_files()
{
    // xrender is multi-threaded
    // "this" is passed in with std::ref but never modified
    m_files = m_file_lst.xrender<File>( [this](const xstring& item)  
    {

        xstring nitem;
        if (!m_use_full_path)
            nitem = '.' + item(static_cast<double>(os.pwd().size()), 0);
        else
            nitem = item;

        File file (nitem);

        if (file.data(4, 0).has_non_ascii()) 
        {
            file.non_ascii = true;
            return file;
        }

        xvector<xstring> splits;
        xvector<xstring> finds;
        unsigned long long int counter = 0;
        for (const xstring& line : file.data.split('\n'))
        {
            counter++;
            File::Splits sp;
            splits = line.split(m_rex, m_icase);
            finds = line.findall(m_rex, m_icase);
            if (finds.size() > 0)
            {
                for (size_t i = 0; i < splits.size(); i++)
                {
                    sp.rex_off << splits[i];
                    if (i < finds.size()) {
                        sp.rex_on << finds[i];
                        file.matches = true;
                    }
                }
            }
            sp.line_num = counter;
            file.lines << sp;
        }
        return file;
    });
}

void Core::single_core_scan()
{
    m_file_lst = os.dir(m_directory, 'r', 'f');
    for (const xstring& item : m_file_lst) 
    {
        xstring nitem;
        if (!m_use_full_path)
            nitem = '.' + item(static_cast<double>(os.pwd().size()), 0);
        else
            nitem = item;

        File file(nitem);

        if (file.data(4, 0).has_non_ascii())
        {
            file.non_ascii = true;
            continue;
        }

        xvector<xstring> splits;
        xvector<xstring> finds;
        unsigned long long int counter = 0;
        for (const xstring& line : file.data.split('\n'))
        {
            counter++;
            File::Splits sp;
            splits = line.split(m_rex, m_icase);
            finds = line.findall(m_rex, m_icase);
            if (finds.size() > 0)
            {
                for (size_t i = 0; i < splits.size(); i++)
                {
                    sp.rex_off << splits[i];
                    if (i < finds.size()) {
                        sp.rex_on << finds[i];
                        file.matches = true;
                    }
                }
            }
            sp.line_num = counter;
            file.lines << sp;
        }
        m_files << file;
    }
}

void Core::print()
{
    this->print_divider();
    for (const File& file : m_files)
    {
        if (file.non_ascii)
            continue;
        file.print(&m_line_tracker_on, sys('n'));
    };

    this->print_divider();
    bool found_non_ascii = false;
    for (const File& file : m_files)
    {
        if (file.non_ascii) {
            if(sys('n'))
                cout << "Non-ASCII: " << file.path << endl;
            found_non_ascii = true;
        }
    }
    if (found_non_ascii)
        this->print_divider();
}

void Core::print_divider() const
{
    cout << cc::blue << xstring(os.console_size()[0], '=') << cc::reset;
}
