
#pragma once

#define  NOMINMAX
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

// Radicalware Libs
#include "Macros.h"
#include "Nexus.h"
#include "OS.h"
#include "xmap.h"
#include "SYS.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

using usint = unsigned short int;

class Core
{
public:
	Core(int FnArgC, char** FasArgv);
	~Core(); 
	void AquireData();
	void ParseRegexFind();
	void Print() const;
private:
	void PrintFind() const;
	void PrintReplace() const;

	static xstring GetPipedSTDIN();
	RA::SYS MoSYS;
	xstring MsFileData;
    bool MbPipedData = false;
    xvector<xvector<xstring>> MsParsedData;
	std::regex MsFinderRex;
};
