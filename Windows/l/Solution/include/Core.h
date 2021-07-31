
#pragma once

#pragma once

#include<Windows.h>
#include<iostream>
#include<vector>
#include<string>
#include<utility>
#include<regex>
#include<functional> 
#include<algorithm>
#include<locale>
#include<initializer_list>
#include<sstream>
#include<set>

// Radicalware Generic Libs
#include "Nexus.h"
#include "OS.h"

// Radicalware eXtended STL Libs
#include "xstring.h"
#include "xvector.h"
#include "xmap.h"

// Modded Object
#include "cc.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

using uint  = unsigned int;
using usint = unsigned short int;


class Core
{
public:
	enum DirType
	{
		DIR,
		TXT,
		EXE,
		INTER
	};
	struct Options
	{
		xstring MsDir;
		bool MbDirStyle     = false;
		bool MbShowAllFiles = false;
		bool MbSortBySize   = false;

		bool MbHelp           = false;
		Options() {}
	};

private:
	struct Files 
	{
		// structurally it would make sense to have the hidden identification in the 
		// Files struct but that would caues me to need to loop the *dir_items
		// and them loop again to mod the Files Struct with the hidden or not functions
		// and values. I wanted to do it in one for-loop to enhance speed at the
		// expense of beutification. 

		xstring* MsNamePtr;
		DirType MoType;
		bool MbHidden;
		Files();
		Files(xstring* FsNamePtr, const DirType& FoType);
	};
	// ----------------------------------------
	// calculated in the constructor
	bool MbDirStyle = false;
	bool MbHiddenValues = false;
	Options MoOptions;
	// ----------------------------------------

	RE2 MoProjectFilesRex = R"(\.(sln|pro)$)";
	RE2 MoExecutableFilesRex  = R"(\.(ps1|rb|com|exe|EXE|bat|cmd|vbs|vbe|js|jse|wsf|wsh|msc|py|pyw|cpl)([\"\s]?)$)";
	xstring MsExecutableExtension;

    Nexus<xvector<xstring>> MvsNexusVector;

	xvector<Core::Files> MvoDirectories;
	xvector<Core::Files> MvoFiles;

	xvector<xstring> MvsFiles;
    xvector<xstring> MvsDirectories;
	xstring MsExample;

	usint MnSpaceSize = 2;
	int MnConsoleWidth = OS::GetConsoleSize()[0];
	xstring MsEmptyString;
	bool MbViewAllFiles = false;

public:
	Core(int FnArgC, char** FasArgv);
	~Core(); 
	bool MbExists = false;
private:
	bool AddDirectoryItems(xstring& FsItem);
	void Trim(xstring& FsItem);
	void SortFiles(xvector<Files>& FvoFiles);

	bool GetIsSystemHidden(const xstring& FsItem);
	bool GetIsUserHidden(const xstring& FsItem);

	void ContinuedPrintFiles(xvector<Files>& FvoDirectory);
	void PrintLsStyleSize(xvector<Files*>& FvoFiles);
	void PrintLsStyleAlpha(xvector<Files*>& FvoFiles);

	void PrintFileOrDirectory(const Files* FoItemPtr) const;
	void PrintFilesOrDirectories(const xvector<Files>& FvoDirectoryList) const;

	void AddWordSpace(size_t FnCount) const;
public:

	void PrintLsStyle();
	void PrintDirStyle();

	Core::Options GetOptions() const;
};
