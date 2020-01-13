#include "File.h"


File::File()
{
}

File::File(const File& file)
{
    this->operator=(file);
}

File::File(const xstring& i_path, bool ibinary_search_on)
{
    path = i_path;
    binary_search_on = ibinary_search_on;
    if (binary_search_on)
    {
#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
        bool open_failure = false;
        try {
            data = OS::Fast_Read(path);
        }
        catch (std::runtime_error&) {
            open_failure = true;
        }

        if (open_failure)
        {
            try {
                data = OS::Stream_Read(path);
            }
            catch (std::runtime_error& errstr) {
                this->data.clear();
                this->err = errstr.what();
            }
        }
#else
        try {
            data = OS::Stat_Read(path);
        }   
        catch (std::runtime_error & errstr) {
            this->data.clear();
            this->err = errstr.what();
        }
#endif
    }
    else { // not binary searching
        try {
            data = OS::Fast_Read(path);
        }
        catch (std::runtime_error& errstr) 
        {
            this->data.clear();
            this->err = errstr.what();// new xstring(errstr.what());
        }
    }
}

File::~File()
{
    //if (err != nullptr) 
    //    delete err;
}

void File::operator=(const File& file)
{
    this->path = file.path;
    this->data = file.data;
    this->lines = file.lines;
    this->binary = file.binary;
    this->matches = file.matches;
    this->binary_search_on = file.binary_search_on;
}


void File::print()
{
    bool printed = false;
    char spacer[3];
    if (!piped_data)
        strncpy(spacer, "\n\n\0", 3);
    else
        strncpy(spacer, "\0", 3);

    if (this->matches && !this->binary)
    {
        this->print_divider();
#pragma warning (suppress : 6053) // Above I enusre we get null bytes for spacer
        cout << Color::Mod::Bold << Color::Cyan << ">>> FILE: >>> " << this->path.sub(m_backslash_rex, "\\\\") << spacer << Color::Mod::Reset;
        printed = true;
    }

    bool line_match = false;
    for (const xstring& line : lines)
    {
        if (line.size())
            line.print();
    }
    if(printed && !piped_data)
        cout << '\n';
}

void File::print_divider() const
{
    cout << Color::Blue << xstring(OS::Console_Size()[0], '-') << Color::Mod::Reset;
}
