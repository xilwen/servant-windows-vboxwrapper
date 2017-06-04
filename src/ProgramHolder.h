#pragma once
#include <windows.h>
#include <string>

class ProgramHolder
{
public:
	ProgramHolder();

	virtual ~ProgramHolder();

	void setCmdLine(const std::wstring& cmdLine);

	std::wstring getCmdLine();

	void run();

	bool isRunning();

	std::string *getStdOut();

	void clearStdOut();

private:
	const std::string className = "ProgramHolder";
	HANDLE childStdInWrite,
		childStdInRead,
		childStdOutWrite,
		childStdOutRead;
	SECURITY_ATTRIBUTES securityAttributes;
	PROCESS_INFORMATION processInformation;
	std::wstring cmdLine;
	std::string stdOut;
	DWORD exitCode;
	bool running = false;

	void stdOutPipeRunner();
};

