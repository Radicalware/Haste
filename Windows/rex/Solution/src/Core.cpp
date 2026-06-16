#include "Core.h"
#include "Timer.h"
#include "OS.h"


Core::Core(int FnArgC, char** FasArgv) 
{
	Begin();
    MoSYS.AddAlias('c', "--Colorless");
	MoSYS.SetArgs(FnArgC, FasArgv);
	Rescue();
}

Core::~Core()
{
}

void Core::AquireData()
{
	Begin();
	auto LoNexus = Nexus<xstring>();
	LoNexus.AddTask(&GetPipedSTDIN);
	auto LoTimer = RA::Timer();
	while (!LoNexus.BxDone() && LoTimer.GetElapsedTimeMilliseconds() < 100) {
		true;
	}
	MsFileData = LoNexus.GetAll()[0];
	if (MsFileData.Size())
		MbPipedData = true;
	else
	{
		auto LsFileName = MoSYS.ArgV(1);
		MsFileData = RA::OS::ReadFile(LsFileName);
	}
	Rescue();
}

void Core::ParseRegexFind()
{
	Begin();

	auto LoRegex = (MbPipedData) ? MoSYS.ArgV(1) : MoSYS.ArgV(2);
	MsFinderRex = std::regex('(' + LoRegex + ')');
	MsParsedData = MsFileData
		.Split('\n')
		.ForEachThread<xvector<xstring>>([this](const xstring& Str) {return Str.InclusiveSplit(MsFinderRex); });
	Rescue();
}

void Core::Print() const
{
	Begin();
	auto LnInputs = (int)MbPipedData + (MoSYS.ArgC() - 1);
	if (LnInputs == 2)
		PrintFind();
	else
		PrintReplace();
	Rescue();
}

void Core::PrintFind() const
{
	Begin();
	for (const auto& LvLine : MsParsedData)
	{
		if (!LvLine)
			continue;
		bool LbRed = LvLine[0].Match(MsFinderRex);
		for (const auto& LsCapture : LvLine)
		{
			if (LbRed && !MoSYS.Has('c'))
				printf(RED "%s" WHITE, LsCapture.Ptr());
			else
				printf("%s", LsCapture.Ptr());
			LbRed = !LbRed;
		}
		cout << WHITE;
		cout << '\n';
	}
	Rescue();
}

void Core::PrintReplace() const
{
	Begin();
	for (const auto& LvLine : MsParsedData)
	{
		if (!LvLine)
			continue;
		bool LbRed = LvLine[0].Match(MsFinderRex);
		for (const auto& LsCapture : LvLine)
		{
			if (LbRed && !MoSYS.Has('c'))
			{
                const auto LsReplaced = MbPipedData ? MoSYS.ArgV(2) : MoSYS.ArgV(3);
                printf(RED "%s" WHITE, LsReplaced.Ptr());
			}
			else
				printf("%s", LsCapture.Ptr());
			LbRed = !LbRed;
		}
		cout << WHITE;
		cout << '\n';
	}
	Rescue();
}

xstring Core::GetPipedSTDIN()
{
	Begin();
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD bytesAvailable = 0;
	if (!PeekNamedPipe(hStdin, nullptr, 0, nullptr, &bytesAvailable, nullptr))
		return {}; // stdin closed or not a pipe

	if (bytesAvailable == 0) // No real input available
		return {};

	xstring LsReturn;
	xstring LsLine;
	while (std::getline(std::cin, LsLine)) {
		LsReturn += LsLine;
	}
	return LsReturn;
	Rescue();
}