#include "Core.h"


Core::Core(int FnArgC, char** FasArgv)
{

	for (int i = 1; i < FnArgC; i++) 
	{
		if (FasArgv[i][0] == '-') 
		{
			if (strcmp(FasArgv[i], "--help") == 0 || strcmp(FasArgv[i], "-h") == 0) {
				MoOptions.MbHelp = true;
				return;
			}

			if (strcmp(FasArgv[i], "--all") == 0  || strcmp(FasArgv[i], "-a") == 0)
				MoOptions.MbShowAllFiles = true;

			if (strcmp(FasArgv[i], "--dir") == 0  || strcmp(FasArgv[i], "-d") == 0)
				MoOptions.MbDirStyle = true;

			if (strcmp(FasArgv[i], "--size") == 0 || strcmp(FasArgv[i], "-s") == 0)
				MoOptions.MbSortBySize = true;
		}
		else {
			MoOptions.MsDir = FasArgv[i];
		}
	}

	if (MoOptions.MsDir.size())
		MbExists = OS::Has(MoOptions.MsDir);
	else {
		MoOptions.MsDir = OS::PWD();
		MbExists = true;
	}


	// >>> use the code below to dynamically get the executable values <<<
	//m_exe_ext = os.RunConsoleCommand("echo %PATHEXT%").Read();
	//m_exe_ext = m_exe_ext.Sub(R"(;\.)", "|.")(0, -1);
	//m_exe_ext.insert(m_exe_ext.begin(), '(');
	//m_exe_ext += ")$";
	//m_exe_ext = m_exe_ext.ToLower();
}

Core::~Core()
{
}

Core::Files::Files()
{
}

Core::Files::Files(xstring* FsNamePtr, const DirType& FoType):
	MsNamePtr(FsNamePtr), MoType(FoType)
{
}

bool Core::AddDirectoryItems(xstring& FsItem)
{
	if (MoOptions.MbShowAllFiles) {
		this->Trim(FsItem);
		return true;
	}
	else {
		if (!this->GetIsSystemHidden(FsItem))
			return false;
		this->Trim(FsItem);
		if (!this->GetIsUserHidden(FsItem))
			return false;
	}
	return true;
}

void Core::Trim(xstring& FsItem)
{
	if (FsItem[FsItem.size() - 1] == '/' || FsItem[FsItem.size() - 1] == '\\')
		FsItem = FsItem(0, -1).Sub(R"(^.*[/\\])", "");
	else
		FsItem = FsItem.Sub(R"(^.*[/\\])", "");

	FsItem.Trim();
	if (FsItem.Scan(' '))
		FsItem = '"' + FsItem + '"';
	else
		FsItem = ' ' + FsItem + ' ';
}

void Core::SortFiles(xvector<Files>& FvoFiles)
{
	if (MoOptions.MbSortBySize) {
		FvoFiles.Sort([](const Files& One, const Files& Two) -> bool
			{return One.MsNamePtr->size() > Two.MsNamePtr->size(); }
		);
	}
	else {
		FvoFiles.Sort([](const Files& One, const Files& Two) -> bool
			{return *One.MsNamePtr < *Two.MsNamePtr; }
		);
	}
}

bool Core::GetIsSystemHidden(const xstring& FsItem)
{
	DWORD LnFileAttribute = GetFileAttributesA(FsItem.c_str());
	if ((FILE_ATTRIBUTE_SYSTEM & LnFileAttribute) || (FILE_ATTRIBUTE_HIDDEN & LnFileAttribute)) 
		return false;
	
	return true;
}

bool Core::GetIsUserHidden(const xstring& FsItem)
{
	if ((FsItem[1] == '.') || (FsItem[1] == '_'))
		return false;

	return true;
}


void Core::ContinuedPrintFiles(xvector<Files>& FvoDirectory)
{
	const int LnSpaceSize = 2;

	this->SortFiles(FvoDirectory);

	size_t LnTotalNumberOfCharacters = 0;
	for (Files& file : FvoDirectory)
		LnTotalNumberOfCharacters += file.MsNamePtr->size() + MnSpaceSize;

	xvector<Files*> pfiles = FvoDirectory.GetPtrs(); // todo: undo this data creation

	if(MoOptions.MbSortBySize && LnTotalNumberOfCharacters > MnConsoleWidth){
		this->PrintLsStyleSize(pfiles);
	}
	else if(LnTotalNumberOfCharacters > MnConsoleWidth){
		this->PrintLsStyleAlpha(pfiles);
	}
	else {
		this->PrintFilesOrDirectories(FvoDirectory);
	}
	cout << CC::Reset;
}

void Core::PrintLsStyleSize(xvector<Files*>& FvoFiles)
{
	double LnRowCount = 2; // double to get processed by slice;
	size_t LnRowStringSize = 0;
	while (true) {

		// keep splitting into new columns until everything fits into the console
		pint LnLongPoint = 0;
		for (double idx = 0; idx < LnRowCount; idx++)
		{
			// get a list of files where we have the longest filename
			// the longest filename is always at the start of a col (we flip it later)
			// then we count up each row increment in reverse order
			xvector<Files*> LvoFiles = FvoFiles(-idx, 0, -LnRowCount, 's');
			LnRowStringSize = 0;

			for (Files* file : LvoFiles)
				LnRowStringSize += 2 + file->MsNamePtr->size();

			if (LnRowStringSize > MnConsoleWidth)
				break;
		}

		if (LnRowStringSize < MnConsoleWidth)
		{
			if (FvoFiles.size() % static_cast<size_t>(LnRowCount) != 0) {
				if (LnRowStringSize + FvoFiles[0]->MsNamePtr->size() + MnSpaceSize < MnConsoleWidth)
					break;
			}
			else {
				break;
			}
		}
		LnRowCount++;
	}

	xvector<Files*> LvoLastRow = FvoFiles(-(LnRowCount - 1), 0, -LnRowCount, 's');
	LvoLastRow << FvoFiles[0];

	for (double idx = 0; idx < LnRowCount; idx++)
	{
		int Count = 0;
		for (Files* LoFilePtr : FvoFiles(-idx, 0, -LnRowCount, 's'))
		{
			this->PrintFileOrDirectory(LoFilePtr);
			this->AddWordSpace(LvoLastRow[Count]->MsNamePtr->size() - LoFilePtr->MsNamePtr->size() + 1);
			Count++;
		}
		if (idx < LnRowCount - 1) cout << '\n';
	}
}

void Core::PrintLsStyleAlpha(xvector<Files*>& FvoFiles)
{
	double LnRowCount = 2; // double to get processed by slice;
	xvector<size_t> LvnLongestColumnValues; // list of the biggest string size in each col
	size_t LnRowStringLength = 0;
	xvector<xvector<Files*>> LvvoFileStacks;
	while (true) {

		// keep splitting into new columns until everything fits into the console
		pint LnLongPoint = 0;
		LvnLongestColumnValues.clear();
		LvvoFileStacks.clear();
		LvnLongestColumnValues.resize(FvoFiles.size() / static_cast<size_t>(LnRowCount) + 1, 0);

		for (double idx = 0; idx < LnRowCount; idx++) // slice the files list to get a list of each row
			LvvoFileStacks << FvoFiles(idx, 0, LnRowCount, 's');

		for (const xvector<Files*>& row_vals : LvvoFileStacks) { // for each row
			int count_down = 0;
			for (int col_idx = 0; col_idx < row_vals.size(); col_idx++) { // for each col of that row
				if (LvnLongestColumnValues[col_idx] < row_vals[col_idx]->MsNamePtr->size()) {
					LvnLongestColumnValues[col_idx] = row_vals[col_idx]->MsNamePtr->size(); // find the longest col value
				}
			}
		}

		if (LvnLongestColumnValues.GetSum() + (2 * LvnLongestColumnValues.size() - 2) < MnConsoleWidth)
			break;
		LnRowCount++;
	}

	int Counter = 0;
	for (const xvector<Files*>& LvoFiles : LvvoFileStacks) // for loop prints (no multithreading)
	{
		int LnCountRight = 0;
		for (const Files* LoFilesPtr : LvoFiles) { // for each col value
			this->PrintFileOrDirectory(LoFilesPtr); // prints the files
			if (static_cast<size_t>(1) + LnCountRight < LvnLongestColumnValues.size())
				this->AddWordSpace(LvnLongestColumnValues[LnCountRight] - LoFilesPtr->MsNamePtr->size() + 1);
			LnCountRight++;
		}
		Counter++;
		if(Counter != LvvoFileStacks.size())
			cout << '\n';
	}
}

void Core::PrintFileOrDirectory(const Files* FoItemPtr) const
{
	if (FoItemPtr->MoType == TXT)
		cout << CC::White << *FoItemPtr->MsNamePtr;
	else if (FoItemPtr->MoType == DIR)
		cout << CC::Cyan << *FoItemPtr->MsNamePtr;
	else if (FoItemPtr->MoType == EXE)
		cout << CC::Green << *FoItemPtr->MsNamePtr;
	else if (FoItemPtr->MoType == INTER)
		cout << CC::Magenta << *FoItemPtr->MsNamePtr;
	
}

void Core::PrintFilesOrDirectories(const xvector<Files>& FvoDirectoryList) const
{
	for (auto& item : FvoDirectoryList) {
		if (item.MoType == TXT)
			cout << CC::White << *item.MsNamePtr;
		else if (item.MoType == DIR)
			cout << CC::Cyan << *item.MsNamePtr;
		else if (item.MoType == EXE)
			cout << CC::Green << *item.MsNamePtr;
		else if (item.MoType == INTER)
			cout << CC::Magenta << *item.MsNamePtr;
		cout << "  ";
	}
}

void Core::AddWordSpace(size_t FnCount) const 
{
	if (FnCount < 2) FnCount = 1;
	char* LsBufferPtr = new char[FnCount+2];
	for (unsigned int i = 0; i <= FnCount; i++)
		LsBufferPtr[i] = ' ';
	LsBufferPtr[FnCount + 1] = '\0';
	std::cout << LsBufferPtr;
	delete[] LsBufferPtr;
}

void Core::PrintLsStyle()
{

	if (OS::HasFile(MoOptions.MsDir)) {
		cout << OS::FullPath(MoOptions.MsDir) << endl;
		return;
	}

	//MvsNexusVector.AddJob(OS::Dir(MoOptions.MsDir, 'f'));
	//MvsNexusVector.AddJob(OS::Dir(MoOptions.MsDir, 'd'));
	MvsNexusVector.AddJob((xvector<xstring>(*)(xstring, char))&OS::Dir, MoOptions.MsDir, 'f');
	MvsNexusVector.AddJob((xvector<xstring>(*)(xstring, char))&OS::Dir, MoOptions.MsDir, 'd');
	MvsNexusVector.WaitAll();
	MvsNexusVector.WaitAll();
    MvsFiles = std::move(MvsNexusVector(0).GetValue());
    MvsDirectories = std::move(MvsNexusVector(1).GetValue());
    MvsNexusVector.Clear();

	for (xstring& LsFile : MvsFiles)
	{
		if (!this->AddDirectoryItems(LsFile))
			continue;

		if (LsFile.Scan(MoExecutableFilesRex))
			MvoFiles << Files(&LsFile, EXE);
		else if (LsFile.Scan(MoProjectFilesRex))
			MvoFiles << Files(&LsFile, INTER);
		else
			MvoFiles << Files(&LsFile, TXT);
	}

	for (xstring& LsDir : MvsDirectories) {
		if (!this->AddDirectoryItems(LsDir))
			continue;
		MvoDirectories << Files(&LsDir, DIR);
	}

	if (MvoDirectories.size() == 0 && MvoFiles.size() == 0) {
		cout << ">> Empty Directory <<\n";
        return;
	}
	
	if (MvoFiles.size()) cout << '\n';
	this->ContinuedPrintFiles(MvoFiles);
	
	if (MvoFiles.size()) cout << '\n';
	if (MvoDirectories.size()) cout << '\n';
	this->ContinuedPrintFiles(MvoDirectories);
	if (MvoDirectories.size() > 0) cout << '\n';

	cout << '\n';
}

void Core::PrintDirStyle()
{
	OS LoOS;
	if (LoOS.HasFile(MoOptions.MsDir)) {
		cout << LoOS.RunConsoleCommand("DIR " + MoOptions.MsDir).Read();
        return;
	}

	xstring LsDirectoryOut;
	cout << "\n  ";
	if (MoOptions.MsDir.size())
		LsDirectoryOut = LoOS.RunConsoleCommand("DIR " + MoOptions.MsDir).Read();
	else
		LsDirectoryOut = LoOS.RunConsoleCommand("DIR " + LoOS.PWD()).Read();

	MvsFiles = LsDirectoryOut.Split(R"(^ Directory of.*$)")[1].Trim().Split('\n')(2, -2);

	if(MoOptions.MbShowAllFiles){
		for (xstring& LsFile : MvsFiles) {
			if (!LsFile.size())
				continue;

			LsFile += '\n';
			if (LsFile.Scan("^.{25}DIR\\>")) {
				MvoDirectories << Files(&LsFile, DIR);
			}
			else if (LsFile.Scan(MoExecutableFilesRex))
				MvoFiles << Files(&LsFile, EXE);
			else if (LsFile.Scan(MoProjectFilesRex))
				MvoFiles << Files(&LsFile, INTER);
			else
				MvoFiles << Files(&LsFile, TXT);
		}
	}
	else {
		for (xstring& LsFile : MvsFiles) {
			if (!LsFile.size())
				continue;
			if (!LsFile.Match(R"(^.*(AM|PM).*((\<DIR\>)|(\d+))\s+[\._].*$)")) {
				LsFile += '\n';
				if (LsFile.Scan("^.{25}DIR\\>")) {
					MvoDirectories << Files(&LsFile, DIR);
				}
				else if (LsFile.Scan(MoExecutableFilesRex))
					MvoFiles << Files(&LsFile, EXE);
				else if (LsFile.Scan(MoProjectFilesRex))
					MvoFiles << Files(&LsFile, INTER);
				else
					MvoFiles << Files(&LsFile, TXT);
			}
		}
	}

	if (MvoDirectories.size() == 0 && MvoFiles.size() == 0) {
		cout << ">> Empty Directory <<\n";
        return;
	}

	this->PrintFilesOrDirectories(MvoDirectories);
	this->PrintFilesOrDirectories(MvoFiles);

	cout << '\n';
}

Core::Options Core::GetOptions() const
{
	return MoOptions;
}


