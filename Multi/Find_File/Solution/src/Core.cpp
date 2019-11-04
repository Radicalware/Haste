#include "Core.h"

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
    m_rex += input.sub(R"((\\\\|/)+)", "[\\\\]+(?=[^\\\\]|$)") + ')';
}

void Core::swap_split()
{
    m_swap_split = true;
}

void Core::gather_files()
{
    m_file_lst = os.dir(m_directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return os.dir(dir, 'r', 'f'); }).expand();
    m_file_lst += os.dir(m_directory, 'f');
    cout << cc::cyan << "Files in Dir: " << m_file_lst.size() << cc::reset << endl;
}

void Core::find_matching_files()
{
    // xrender is multi-threaded
    // "this" is passed with std::ref in but never modified
    xvector<Splits> file_lst = m_file_lst.xrender<Splits>([this](xstring& item)
    {
        xvector<xstring> splits;
        xvector<xstring> finds;

        if (!m_use_full_path)
            item = '.' + item.substr(m_directory.size(), item.size() - m_directory.size());

        splits = item.split(m_rex, m_icase);
        finds = item.findall(m_rex, m_icase);
        Splits sp;
        if (finds.size() > 0) {
            for (size_t i = 0; i < splits.size(); i++) {

#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))

                if(m_swap_split)
                    sp.rex_off << splits[i].sub(R"(\\\\)", R"(/)");
                else
                    sp.rex_off << splits[i].sub(R"(\\\\)", R"(\)");
                if (i < finds.size()) {
                    if(m_swap_split)
                        sp.rex_on << finds[i].sub(R"(\\\\)", R"(/)");
                    else
                        sp.rex_on << finds[i].sub(R"(\\\\)", R"(\)");
                }
#else // ********************* NIX ***************************************
                if (m_swap_split)
                    sp.rex_off << splits[i].sub(R"(/)", R"(\\)");
                else
                    sp.rex_off << splits[i];

                if (i < finds.size()) {
                    if (m_swap_split)
                        sp.rex_on << finds[i].sub(R"(/)", R"(\\)");
                    else
                        sp.rex_on << finds[i];
                }
#endif // *****************************************************************

            }
            sp.not_found = true;
        }
        return sp;
    });

    for(const Splits& files : file_lst) {
        if (files.rex_on.size() > 0) {
            for (size_t i = 0; i < files.rex_off.size(); i++) {
                // files.rex_off[i];
                cout << cc::white << files.rex_off[i];
                if (i < files.rex_on.size())
                    // files.rex_on[i];
                    cout << cc::green << files.rex_on[i];
            }
            cout << '\n';
        }
    };
}

void Core::single_core_scan()
{
    m_file_lst = os.dir(m_directory, 'r', 'f');
    
	xvector<xstring> splits;
	xvector<xstring> finds;

    for (xstring& item : m_file_lst) {
		if(!m_use_full_path)
			item = '.' + item.substr(m_directory.size(), item.size() - m_directory.size());

		splits = item.split(m_rex, m_icase);
		finds = item.findall(m_rex, m_icase);
		if (finds.size() > 0) {
			for (size_t i = 0; i < splits.size(); i++) {
                // splits[i].sub(R"(\\\\)", R"(\)");
                cout << cc::white << splits[i].sub(R"(\\\\)", R"(\)");
                if (i < finds.size())
                    // finds[i].sub(R"(\\\\)", R"(\)");
                    cout << cc::green << finds[i].sub(R"(\\\\)", R"(\)");
            }
			cout << '\n';
		}
    }
}
