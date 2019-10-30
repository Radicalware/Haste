#include "Core.h"




// normally I would put the data below into a struct
// but because I have the same data used in a non-multi-threading function
// and because this data is only sent to a function one time together
// I decided to forgo the struct

void Core::scan_dirs(std::mutex& mx, 
	xvector<xstring>& file_lst, const xstring& rex, // main file data
	int core_count, int zone,  // multi-threading data
	size_t dir_size, bool use_full_path // display data
){
	int showcase = static_cast<int>(file_lst.size()) / core_count;
	int extra = 0;
	if (zone == core_count - 1)
		extra = file_lst.size() % core_count;

	xvector<xstring> splits;
	xvector<xstring> finds;

	for (xstring& item : file_lst(zone * showcase, (zone * showcase) + extra)) {

		if(!use_full_path)
			item = '.' + item.substr(dir_size, item.size() - dir_size);

		splits = item.split(rex);
		finds = item.findall(rex);

		if (finds.size() > 0) {
			for (size_t i = 0; i < splits.size(); i++) {
				mx.lock(); // mutex is a reference so it spans across all threads
				cout << cc::white << splits[i].sub(R"(\\\\)", R"(\)");
				if (i < finds.size())
					cout << cc::green << finds[i].sub(R"(\\\\)", R"(\)");
				mx.unlock(); // mutex version is not unique to enhance speed
			}
			cout << '\n';
		}
	}
}

void Core::parse_as_regex(bool input)
{
	m_parse_as_regex = input;
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
    m_file_lst = os.dir(m_directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return os.dir(dir, 'r', 'f'); }).expand<xstring>();
    m_file_lst += os.dir(m_directory, 'f');
}

void Core::find_matching_files()
{
    // xrender is multi-threaded
    // "this" is passed in but never modified
    xvector<Splits> file_lst = m_file_lst.xrender<Splits>([this](xstring& item) {

        xvector<xstring> splits;
        xvector<xstring> finds;

        if (!m_use_full_path)
            item = '.' + item.substr(m_directory.size(), item.size() - m_directory.size());

        splits = item.split(m_rex);
        finds = item.findall(m_rex);
        Splits sp;
        if (finds.size() > 0) {
            for (size_t i = 0; i < splits.size(); i++) {
                sp.rex_off << splits[i].sub(R"(\\\\)", R"(\)");
                if (i < finds.size())
                    sp.rex_on << finds[i].sub(R"(\\\\)", R"(\)");
            }
        }
        return sp;
    });

    file_lst.proc([](Splits& files) {
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
    });
}

void Core::init_thrads()
{
    // multi-thread each top-tier directory scan.
    m_file_lst = os.dir(m_directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return os.dir(dir, 'r', 'f'); }).expand<xstring>();
    m_file_lst += os.dir(m_directory, 'f');

	const int cores = std::thread::hardware_concurrency();
	for (int i = 0; i < cores; i++)
		m_threads.emplace_back(Core::scan_dirs, ref(m_mutex), ref(m_file_lst), ref(m_rex), cores, i, m_directory.size(), m_use_full_path);
}

void Core::join_thrads()
{
	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
		m_threads[i].join();
}


void Core::single_core_scan()
{
    m_file_lst = os.dir(m_directory, 'r', 'f');
    //m_file_lst = os.dir(m_directory, 'd').xrender<xvector<xstring>>([](xstring& dir) { return os.dir(dir, 'r', 'f'); }).expand<xstring>();
    //m_file_lst += os.dir(m_directory, 'f');

	xvector<xstring> splits;
	xvector<xstring> finds;

    for (xstring& item : m_file_lst) {
		if(!m_use_full_path)
			item = '.' + item.substr(m_directory.size(), item.size() - m_directory.size());

		splits = item.split(m_rex);
		finds = item.findall(m_rex);
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
