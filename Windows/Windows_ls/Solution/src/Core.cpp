
#include "LibLoc.h"

#include "Core.h"


Core::Core(int argc, char** argv)
{

	for (int i = 1; i < argc; i++) 
	{
		if (argv[i][0] == '-') 
		{
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
				m_options.help = true;
				return;
			}

			if (strcmp(argv[i], "--all") == 0  || strcmp(argv[i], "-a") == 0)
				m_options.show_all_files = true;

			if (strcmp(argv[i], "--dir") == 0  || strcmp(argv[i], "-d") == 0)
				m_options.dir_style = true;

			if (strcmp(argv[i], "--size") == 0 || strcmp(argv[i], "-s") == 0)
				m_options.sort_by_size = true;
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
	else
		item = ' ' + item + ' ';
}

void Core::sort_files(xvector<Files>& file_lst)
{
	if (m_options.sort_by_size) {
		file_lst.sort([](const Files& one, const Files& two) -> bool
			{return one.name->size() > two.name->size(); }
		);
	}
	else {
		file_lst.sort([](const Files& one, const Files& two) -> bool
			{return one.name < two.name; }
		);
	}
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
	if ((item[1] == '.') || (item[1] == '_'))
		return false;

	return true;
}


void Core::continued_print_files(xvector<Files>& dir_lst)
{
	const int space_size = 2;

	this->sort_files(dir_lst);

	size_t total_chars = 0;
	for (Files& file : dir_lst)
		total_chars += file.name->size() + m_space_size;

	xvector<Files*> pfiles = dir_lst.ptrs(); // todo: undo this data creation

	if(m_options.sort_by_size && total_chars > m_console_width){
		this->print_ls_style_size(pfiles);
	}
	else if(total_chars > m_console_width){
		this->print_ls_style_alph(pfiles);
	}
	else {
		this->dir_vals_out(dir_lst);
	}
	cout << cc::reset;
}

void Core::print_ls_style_size(xvector<Files*>& pfiles)
{
	double row_count = 2; // double to get processed by slice;
	size_t row_str_size = 0;
	while (true) {

		// keep splitting into new columns until everything fits into the console
		uint long_point = 0;
		for (double idx = 0; idx < row_count; idx++)
		{
			// get a list of files where we have the longest filename
			// the longest filename is always at the start of a col (we flip it later)
			// then we count up each row increment in reverse order
			xvector<Files*> files = pfiles(-idx, 0, -row_count, 's');
			row_str_size = 0;

			for (Files* file : files)
				row_str_size += 2 + file->name->size();

			if (row_str_size > m_console_width)
				break;
		}

		if (row_str_size < m_console_width)
		{
			if (pfiles.size() % static_cast<size_t>(row_count) != 0) {
				if (row_str_size + pfiles[0]->name->size() + m_space_size < m_console_width)
					break;
			}
			else {
				break;
			}
		}
		row_count++;
	}

	xvector<Files*> last_row = pfiles(-(row_count - 1), 0, -row_count, 's');
	last_row << pfiles[0];

	for (double idx = 0; idx < row_count; idx++)
	{
		int count = 0;
		for (Files* file : pfiles(-idx, 0, -row_count, 's'))
		{
			this->dir_ptr_out(file);
			this->add_word_space(last_row[count]->name->size() - file->name->size() + 1);
			count++;
		}
		if (idx < row_count - 1) cout << '\n';
	}
}

void Core::print_ls_style_alph(xvector<Files*>& pfiles)
{
	double row_count = 2; // double to get processed by slice;
	xvector<size_t> longest_col_values; // list of the biggest string size in each col
	size_t row_str_len = 0;
	xvector<xvector<Files*>> file_stacks;
	while (true) {

		// keep splitting into new columns until everything fits into the console
		uint long_point = 0;
		longest_col_values.clear();
		file_stacks.clear();
		longest_col_values.resize(pfiles.size() / static_cast<size_t>(row_count) + 1, 0);

		for (double idx = 0; idx < row_count; idx++) // slice the files list to get a list of each row
			file_stacks << pfiles(idx, 0, row_count, 's');

		for (const xvector<Files*>& row_vals : file_stacks) { // for each row
			int count_down = 0;
			for (int col_idx = 0; col_idx < row_vals.size(); col_idx++) { // for each col of that row
				if (longest_col_values[col_idx] < row_vals[col_idx]->name->size()) {
					longest_col_values[col_idx] = row_vals[col_idx]->name->size(); // find the longest col value
				}
			}
		}

		if (longest_col_values.sum() + (2 * longest_col_values.size() - 2) < m_console_width)
			break;
		row_count++;
	}

	int counter = 0;
	for (const xvector<Files*>& row_vals : file_stacks) // for loop prints (no multithreading)
	{
		int count_right = 0;
		for (const Files* files : row_vals) { // for each col value
			this->dir_ptr_out(files); // prints the files
			if (static_cast<size_t>(1) + count_right < longest_col_values.size())
				this->add_word_space(longest_col_values[count_right] - files->name->size() + 1);
			count_right++;
		}
		counter++;
		if(counter != file_stacks.size())
			cout << '\n';
	}
}

void Core::dir_ptr_out(const Files* item) const
{
	if (item->type == TXT)
		cout << cc::white << *item->name;
	else if (item->type == DIR)
		cout << cc::cyan << *item->name;
	else if (item->type == EXE)
		cout << cc::green << *item->name;
	else if (item->type == INTER)
		cout << cc::magenta << *item->name;
	
}

void Core::dir_vals_out(const xvector<Files>& dir_lst) const
{
	for (auto& item : dir_lst) {
		if (item.type == TXT)
			cout << cc::white << *item.name;
		else if (item.type == DIR)
			cout << cc::cyan << *item.name;
		else if (item.type == EXE)
			cout << cc::green << *item.name;
		else if (item.type == INTER)
			cout << cc::magenta << *item.name;
		cout << "  ";
	}
}

void Core::add_word_space(size_t count) const 
{
	if (count < 2) count = 1;
	char* char_str = new char[count+2];
	for (unsigned int i = 0; i <= count; i++)
		char_str[i] = ' ';
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

    m_nex_vec.add_job(os.dir(m_options.dir, 'f'));
    m_nex_vec.add_job(os.dir(m_options.dir, 'd'));
    m_str_files = std::move(m_nex_vec(0).value());
    m_str_directories = std::move(m_nex_vec(1).value());
    m_nex_vec.clear();

	for (xstring& file : m_str_files)

	{
		if (!this->add_dir_items(file))
			continue;

		if (file.scan(m_exe_files))
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
	if (os.file(m_options.dir)) {
		cout << os.popen("DIR " + m_options.dir).read();
		exit(0);
	}

	xstring dir_out;
	cout << "\n  ";
	if (m_options.dir.size())
		dir_out = os.popen("DIR " + m_options.dir).read();
	else
		dir_out = os.popen("DIR " + os.pwd()).read();

	m_str_files = dir_out.split(R"(^ Directory of.*$)")[1].strip().split('\n')(2, -2);

	if(m_options.show_all_files){
		for (xstring& item : m_str_files) {
			if (!item.size())
				continue;

			item += '\n';
			if (item.scan("^.{25}DIR\\>")) {
				m_directories << Files(&item, DIR);
			}
			else if (item.scan(m_exe_files))
				m_files << Files(&item, EXE);
			else if (item.scan(m_proj_files))
				m_files << Files(&item, INTER);
			else
				m_files << Files(&item, TXT);
		}
	}
	else {
		for (xstring& item : m_str_files) {
			if (!item.size())
				continue;
			if (!item.match(R"(^.*(AM|PM).*((\<DIR\>)|(\d+))\s+[\._].*$)")) {
				item += '\n';
				if (item.scan("^.{25}DIR\\>")) {
					m_directories << Files(&item, DIR);
				}
				else if (item.scan(m_exe_files))
					m_files << Files(&item, EXE);
				else if (item.scan(m_proj_files))
					m_files << Files(&item, INTER);
				else
					m_files << Files(&item, TXT);
			}
		}
	}

	if (m_directories.size() == 0 && m_files.size() == 0) {
		cout << ">> Empty Directory <<\n";
		exit(0);
	}

	this->dir_vals_out(m_directories);
	this->dir_vals_out(m_files);

	cout << '\n';
}

Core::Options Core::options() const
{
	return m_options;
}


