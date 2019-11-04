#include "File.h"


File::File()
{
    this->path = "Uninitialized";
}

File::File(const File& file)
{
    this->operator=(file);
}

File::File(const xstring& i_path): path(i_path)
{
    data = os.open(path).read();
}

void File::operator=(const File& file)
{
    this->path = file.path;
    this->data = file.data;
    this->lines = file.lines;
    this->non_ascii = file.non_ascii;
    this->matches = file.matches;
}


void File::print(bool* line_tracker_on, bool show_non_matches) const
{
    if (!this->matches) {
        if (*line_tracker_on && show_non_matches) {
            cout << '\n';
            this->print_divider();
            *line_tracker_on = false;
        }
        if(show_non_matches)
            cout << "No Matches: " << this->path << endl;
        return;
    }
    else {
        *line_tracker_on = true;
    }

    this->print_divider();
    cout << cc::cyan << ">>> FILE: >>> " << this->path.sub(R"(\\\\)","\\") << "\n\n" << cc::reset;

    bool line_match = false;
    for (auto& line : lines)
    {
        line_match = line.rex_on.size();
        if (line_match)
            cout << line.line_num << ": ";
        for (size_t i = 0; i < line.rex_off.size(); i++) 
        {
            if(line_match)
                cout << cc::white << line.rex_off[i];
            if (i < line.rex_on.size())
                cout << cc::green << line.rex_on[i];
        }
        if (line_match)
            cout << '\n';
    };
    cout << '\n';
}

void File::print_divider() const
{
    cout << cc::blue << xstring(os.console_size()[0], '=') << cc::reset;
}

File::Splits::Splits()
{
}

File::Splits::Splits(const Splits& split)
{
    rex_on = split.rex_on;
    rex_off = split.rex_off;
    line_num = split.line_num;
}
