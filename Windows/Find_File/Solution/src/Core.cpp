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

void Core::init_thrads()
{
	m_file_lst = os.dir(m_directory, 'r', 'f');


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
	xvector<xstring> splits;
	xvector<xstring> finds;

    for (xstring& item : m_file_lst) {
		if(!m_use_full_path)
			item = '.' + item.substr(m_directory.size(), item.size() - m_directory.size());

		splits = item.split(m_rex);
		finds = item.findall(m_rex);
		if (finds.size() > 0) {
			for (size_t i = 0; i < splits.size(); i++) {
				cout << cc::white << splits[i].sub(R"(\\\\)", R"(\)");
				if (i < finds.size())
					cout << cc::green << finds[i].sub(R"(\\\\)", R"(\)");
			}
			cout << '\n';
		}
    }
}



