#include "File.h"


File::File()
{
}

File::File(const File& file)
{
    this->operator=(file);
}

File::File(const xstring& i_path)
{
    path = i_path;
    if (sys('b')) {
        data = os.read(path);
    }
    else {
        try {
            data = os.fast_text(path);
        }
        catch (std::runtime_error&) {
            this->non_ascii = 1;
            this->binary = true;
        }
    }
}

void File::operator=(const File& file)
{
    this->path = file.path;
    this->data = file.data;
    this->lines = file.lines;
    this->non_ascii = file.non_ascii;
    this->matches = file.matches;
}


void File::print(const xstring& rex)
{
    bool printed = false;
    if (this->matches && !this->non_ascii) {
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
    cout << cc::blue << xstring(os.console_size()[0], '-') << cc::reset;
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
