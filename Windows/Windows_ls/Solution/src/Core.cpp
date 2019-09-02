
#include "LibLoc.h"

#include "Core.h"


Core::Core(int argc, char** argv)
{

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "--all") == 0 || strcmp(argv[i], "-a") == 0)
				m_options.show_all_files = true;
			else if (strcmp(argv[i], "--dir") == 0 || strcmp(argv[i], "-d") == 0)
				m_options.dir_style = true;
		}
		else {
			m_options.dir = argv[i];
		}
	}

	if (!m_options.dir.size())
		m_options.dir = os.pwd();


	// >>> use the code below to dynamically get the executable values <<<
	//m_exe_ext = os.popen("echo %PATHEXT%").read();
	//m_exe_ext = m_exe_ext.sub(R"(;\.)", "|.")(0, -1);
	//m_exe_ext.insert(m_exe_ext.begin(), '(');
	//m_exe_ext += ")$";
	//m_exe_ext = m_exe_ext.lower();

	m_exe_ext = m_exe_files;
}


Core::~Core()
{
}


Core::Files::Files()
{
}

Core::Files::Files(xstring* i_name, const DirType i_type):
	name(i_name), type(i_type) 
{
}


bool Core::add_dir_items(xstring& item)
{
	if (m_options.show_all_files) {
		this->trim(item);
		return true;
	}
	else {
		if (!this->system_hidden(item))
			return false;
		this->trim(item);
		if (!this->user_hidden(item))
			return false;
	}
	return true;
}

void Core::trim(xstring& item)
{
	if (item[item.size() - 1] == '/' || item[item.size() - 1] == '\\')
		item = item(0, -1).sub(R"(^.*[/\\])", "");
	else
		item = item.sub(R"(^.*[/\\])", "");

	item.strip();
	if (item.scan(' '))
		item = '"' + item + '"';
}

bool Core::system_hidden(const xstring& item)
{
	DWORD char_file = GetFileAttributesA(item.c_str());
	if ((FILE_ATTRIBUTE_SYSTEM & char_file) || (FILE_ATTRIBUTE_HIDDEN & char_file)) 
		return false;
	
	return true;
}

bool Core::user_hidden(const xstring& item)
{
	if ((item[0] == '.') || (item[0] == '"' && item[1] == '.'))
		return false;

	return true;
}

void Core::print_File_List(const xvector<Files>& Files_Vec)
{
	for (auto& item : Files_Vec) {
		// standard useage
		if (item.type == TXT)
			cout << cc::white << *item.name;
		else if (item.type == DIR)
			cout << cc::cyan << *item.name;
		else if (item.type == EXE)
			cout << cc::green << *item.name;
		else if (item.type == INTER)
			cout << cc::magenta << *item.name;
	}
}

void Core::continued_print_files(xvector<Files>& dir_lst)
{
	const int space_size = 2;

	dir_lst.sort([](const Files& one, const Files& two) -> bool
		{return one.name->size() > two.name->size(); }
	);

	size_t total_chars = 0;
	for (Files& file : dir_lst)
		total_chars += file.name->size() + m_space_size;

	xvector<Files*> pfiles = dir_lst.ptrs(); // todo: undo this data creation

	double row_count = 1; // double to get processed by slice;
	xstring row_str = "";
	if (total_chars > m_console_width) {
		while (true) {
			// keep splitting into new columns until everything fits into the console
			row_count++;
			if (dir_lst.size() < row_count)
				break;
			uint long_point = 0;
			double idx = 0;
			for (; idx < row_count; idx++) {
					xvector<Files*> files = pfiles(-idx, 0, -row_count, 's');
				row_str = "";
				for (Files* file : files)
					row_str += "  " + *file->name;

				if (row_str.size() > m_console_width)
					break;
			}

			if (row_str.size() < m_console_width) {
				if (dir_lst.size() % static_cast<size_t>(row_count) != 0) {
					if (row_str.size() + pfiles[0]->name->size() + m_space_size < m_console_width)
						break;
				}
				else {
					break;
				}
			}
		}
		auto dir_item_out=[](Files* item)->void {
			// pointer useage
			if (item->type == TXT)
				cout << cc::white << *item->name;
			else if (item->type == DIR)
				cout << cc::cyan << *item->name;
			else if (item->type == EXE)
				cout << cc::green << *item->name;
			else if (item->type == INTER)
				cout << cc::magenta << *item->name;

		};

		xvector<Files*> last_row = pfiles(-(row_count-1), 0, -row_count, 's');
		last_row << pfiles[0];


		for (double idx = 0; idx < row_count; idx++) {

			int count = 0;
			for (Files* file : pfiles(-idx, 0, -row_count, 's')) {
				dir_item_out(file);
				this->dup_char_out(last_row[count]->name->size() - file->name->size() + 1, ' ');
				count++;
			}
			if(idx < row_count-1) cout << '\n';
		}

	}
	else {
		dir_lst.sort([](const Files& one, const Files& two) -> bool
			{return one.name->size() < two.name->size(); }
		);

		for (auto& item : dir_lst) {
			// standard useage
			if (item.type == TXT)
				cout << cc::white << *item.name;
			else if (item.type == DIR)
				cout << cc::cyan << *item.name;
			else if (item.type == EXE)
				cout << cc::green << *item.name;
			else if (item.type == INTER)
				cout << cc::magenta << *item.name;
			cout << "  "; // I decided to not use if/else on "print_File_List()" to enhance speed
		}
	}
	
	cout << cc::white;
}


void Core::dup_char_out(size_t count, char chr) {

	if (count < 2) count = 1;
	char* char_str = new char[count+2];
	for (unsigned int i = 0; i <= count; i++)
		char_str[i] = chr;
	char_str[count + 1] = '\0';
	std::cout << char_str;
	delete[] char_str;
}

void Core::print_ls_style()
{

	if (os.file(m_options.dir)) {
		cout << os.full_path(m_options.dir) << endl;
		exit(0);
	}

	m_str_directories = os.dir(m_options.dir, 'd');
	m_str_files = os.dir(m_options.dir, 'f');

	for (xstring& file : m_str_files)
	{

		if (!this->add_dir_items(file))
			continue;

		if (file.scan(m_exe_ext))
			m_files << Files(&file, EXE);
		else if (file.scan(m_proj_files))
			m_files << Files(&file, INTER);
		else
			m_files << Files(&file, TXT);
	}

	for (xstring& dir : m_str_directories) {
		if (!this->add_dir_items(dir))
			continue;
		m_directories << Files(&dir, DIR);
	}

	if (m_directories.size() == 0 && m_files.size() == 0) {
		cout << ">> Empty Directory <<\n";
		exit(0);
	}
	
	if (m_files.size()) cout << '\n';
	this->continued_print_files(m_files);
	
	if (m_files.size()) cout << '\n';
	if (m_directories.size()) cout << '\n';
	this->continued_print_files(m_directories);
	if (m_directories.size() > 0) cout << '\n';

	cout << '\n';
}

void Core::print_dir_style()
{
	cout << '\n';
	xstring dir_out;
	if (m_arg_folders.size())
		dir_out = os.popen("DIR " + m_arg_folders.join(' ')).read();
	else
		dir_out = os.popen("DIR " + os.pwd()).read();

	m_str_files = dir_out.split(R"(^ Directory of.*$)")[1].strip().split('\n')(2, -2);

	for (xstring& item : m_str_files) {
		if (!item.size())
			continue;

		item += '\n';
		if (item.scan("^.{25}DIR\\>")) {
			m_directories << Files(&item, DIR);
		}
		else if (item.scan(m_exe_ext))
			m_files << Files(&item, EXE);
		else if (item.scan(m_proj_files))
			m_files << Files(&item, INTER);
		else
			m_files << Files(&item, TXT);
	}

	if (m_directories.size() == 0 && m_files.size() == 0) {
		cout << ">> Empty Directory <<\n";
		exit(0);
	}

	this->print_File_List(m_directories);
	this->print_File_List(m_files);

	cout << '\n';
}

Core::Options Core::options()
{
	return m_options;
}


