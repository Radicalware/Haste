#include "Core.h"
#pragma warning( disable : 26812 ) // to allow our rxm enum even though it isn't a class enum

Core::~Core()
{
    if (m_piped_data != nullptr)
        delete m_piped_data;
}

void Core::piped_scan()
{
    m_piped_data = new File;
    File& file = *m_piped_data;

    file.path = "Piped Data";
    if(m_entire)
        file.indent = true;

    xstring line;
    unsigned long int counter = 0;

    while (getline(std::cin, line))
    {
        counter++;
        File::Splits sp;
        sp.splits = line.inclusive_split(m_rex, m_icase, false);
        if (sp.splits.size())
        {
            sp.line_num = counter;
            file.lines << sp;
        }
        else if (m_entire) {
            sp.line_num = counter;
            sp.splits << line;
            file.lines << sp;
        }
    }
}

void Core::set_dir(const xstring& input, bool use_pwd)
{
    if (input.scan(R"(\.\.[/\\])"))
        m_use_full_path = true;
    else if (input.match(R"(^[A-Z]\:.*$)") && !use_pwd)
        m_use_full_path = true;

    m_directory = OS::Full_Path(input);
}

void Core::set_rex(const xstring& input){
    m_rex += input.sub(R"((\\\\|/)+)", "[\\\\]+(?=[^\\\\]|$)") + ')';
}

void Core::set_avoid_regex(const xvector<xstring*>& avoid_lst){
    m_avoid_lst = avoid_lst;
}

void Core::set_full_path_on(){
    m_use_full_path = true;
}

void Core::set_only_name_on(){
    m_only_name_files = true;
}

void Core::set_entire_on(){
    m_entire = true;
}

void Core::set_case_sensitive_on(){
    m_icase = rxm::ECMAScript;
}

void Core::set_binary_on(){
    m_binary_search_on = true;
}

File Core::scan_file(xstring& path, const Core& core, const bool binary_search_on)
{
    if (!core.m_use_full_path)
        path = '.' + path(static_cast<double>(OS::PWD().size()), 0);

    File file(path, binary_search_on);
    if (file.err.size())
    {
        file.data.clear();
        return file;
    }


    bool avoid = static_cast<bool>(core.m_avoid_lst.size());
    bool matches = false;
    if(!avoid)
        matches = file.data.scan(core.m_rex, core.m_icase);

    if ((!avoid) && (!matches))
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

    if (core.m_only_name_files) {
        if (!avoid)
            file.matches = (file.data.scan(core.m_rex, core.m_icase));
        else
            file.matches = (file.data.scan(core.m_rex, core.m_icase) && !file.data.scan_list(core.m_avoid_lst));
    }
    else {
        unsigned long int counter = 0;
        for (const xstring& line : file.data.split('\n'))
        {
            if (avoid) {
                if (line.scan_list(core.m_avoid_lst, core.m_icase))
                    continue;
            }

            counter++;
            File::Splits sp;
            sp.splits = line.inclusive_split(core.m_rex, core.m_icase, false);

            if (sp.splits.size())
            {
                file.matches = true;
                sp.line_num = counter;
                file.lines << sp;
            }
        }
    }
    file.data.clear();
    return file;
}

void Core::multi_core_scan()
{

    m_file_lst = OS::Dir(m_directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return OS::Dir(dir, 'r', 'f'); }).expand();
    m_file_lst += OS::Dir(m_directory, 'f');
    //m_file_lst.join('\n').print();
    cout << cc::cyan << "Files in Dir: " << m_file_lst.size() << cc::reset << endl;

    // xrender is multi-threaded
    // "this" is passed in with std::ref but never modified
    m_files = m_file_lst.xrender<File>(Core::scan_file, std::ref(*this), m_binary_search_on);
}

void Core::single_core_scan()
{
    m_file_lst = OS::Dir(m_directory, 'r', 'f');
    cout << cc::cyan << "Files in Dir: " << m_file_lst.size() << cc::reset << endl;

    m_files = m_file_lst.render<File>(Core::scan_file, std::ref(*this), m_binary_search_on);
}

void Core::print()
{
    if (m_piped_data != nullptr)
    {
        m_piped_data->print(m_rex);

        return;
    }

    if (m_only_name_files) {   // print what the user was looking for
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
            file.print(m_rex);
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
    cout << cc::blue << xstring(OS::Console_Size()[0], '-') << cc::reset;
}
