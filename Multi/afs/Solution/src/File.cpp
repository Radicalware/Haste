#include "File.h"


File::File()
{
}

File::~File()
{
    //if (err != nullptr) 
    //    delete err;
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
            catch (std::runtime_error & errstr) {
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

void File::operator=(const File& file)
{
    this->path = file.path;
    this->data = file.data;
    this->lines = file.lines;
    this->binary = file.binary;
    this->matches = file.matches;
    this->binary_search_on = file.binary_search_on;
}


void File::print(const xstring& rex)
{
    bool printed = false;
    if (this->matches && !this->binary) {
        this->print_divider();
        cout << cc::cyan << ">>> FILE: >>> " << this->path.sub(R"(\\\\)", "\\") << "\n\n" << cc::reset;
        printed = true;
    }

    bool line_match = false;
    for (File::Splits& line : lines)
    {
        if (line.splits.size() == 0)
            continue;

        cout << cc::cyan << line.line_num << ": " << cc::reset;
        bool on = false;
        if (line[0].match(rex))
            on = true;

        double padding = File::Splits::max_line_len - std::to_string(line.line_num).length();
        cout << std::string(static_cast<size_t>(padding), ' ');

        line[0] = line[0].sub(R"(^[\s]*)", "");

        if (line.splits.join().size() > 300) {
            cout << "Line Over 300 Char Limit!\n";
            continue;
        }

        for (size_t i = 0; i < line.splits.size(); i++)
        {
            if (on)
                cout << cc::red << cc::bold << line[i] << cc::reset;
            else
                cout << line[i];
            on = !on;
        }
        cout << "\n";
    };
    if(printed)
        cout << '\n';
}

void File::print_divider() const
{
    cout << cc::blue << xstring(OS::Console_Size()[0], '-') << cc::reset;
}

const double File::Splits::max_line_len = 7;

File::Splits::Splits()
{
}

File::Splits::Splits(const Splits& split)
{
    splits = split.splits;
    line_num = split.line_num;
}

xstring& File::Splits::operator[](size_t val)
{
    return splits[val];
}
