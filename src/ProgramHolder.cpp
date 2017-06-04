#include <stdexcept>
#include <thread>
#include "ProgramHolder.h"
#include "Logger.h"

#define BUFFERSIZE 512

ProgramHolder::ProgramHolder()
{
	Logger::log(className, __func__, InfoLevel::INFO, className + " initializing...");
	securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttributes.bInheritHandle = true;
	securityAttributes.lpSecurityDescriptor = nullptr;
	//TODO Refactor with exception handling(CreatePipe/SetHandle... both return something)
	CreatePipe(&childStdOutRead, &childStdOutWrite, &securityAttributes, 0);
	SetHandleInformation(childStdOutRead, HANDLE_FLAG_INHERIT, 0);
	CreatePipe(&childStdInRead, &childStdInWrite, &securityAttributes, 0);
	SetHandleInformation(childStdInWrite, HANDLE_FLAG_INHERIT, 0);
	ZeroMemory(&processInformation, sizeof(processInformation));
	Logger::log(className, __func__, InfoLevel::INFO, className + " initialized.");
}

ProgramHolder::~ProgramHolder()
{
	Logger::log(className, __func__, InfoLevel::INFO, className + " destruction started.");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	CloseHandle(childStdInWrite);
	CloseHandle(childStdOutRead);
	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);
	Logger::log(className, __func__, InfoLevel::INFO, className + " destructed.");
}

void ProgramHolder::setCmdLine(const std::wstring& cmdLine)
{
	this->cmdLine = cmdLine;
}

void ProgramHolder::run()
{
	if (isRunning())
	{
		Logger::log(className, __func__, InfoLevel::INFO, cmdLine + L" is already running! Rejected.");
		return;
	}
	Logger::log(className, __func__, InfoLevel::INFO, L"Trying to launch " + cmdLine);
	STARTUPINFOW startupinfo;
	int succeed(false);

	ZeroMemory(&processInformation, sizeof(processInformation));
	ZeroMemory(&startupinfo, sizeof(startupinfo));
	startupinfo.cb = sizeof(STARTUPINFOW);
	startupinfo.hStdError = childStdOutWrite;
	startupinfo.hStdOutput = childStdOutWrite;
	startupinfo.hStdInput = childStdInRead;
	startupinfo.dwFlags |= STARTF_USESTDHANDLES;

	succeed = CreateProcessW(nullptr, LPWSTR(cmdLine.c_str()), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr,
		&startupinfo,
		&processInformation);
	if (!succeed)
	{
		Logger::log(className, __func__, InfoLevel::INFO, "Launch failed because " + std::to_string(GetLastError()));
		throw std::runtime_error("Create Process Failed");
		return;
	}
	running = true;
	std::thread stdOutReadThread(&ProgramHolder::stdOutPipeRunner, this);
	stdOutReadThread.detach();
}

std::string *ProgramHolder::getStdOut()
{
	return &stdOut;
}

std::wstring ProgramHolder::getCmdLine()
{
	return cmdLine;
}

bool ProgramHolder::isRunning()
{
	GetExitCodeProcess(processInformation.hProcess, &exitCode);
	return (exitCode == STILL_ACTIVE);
}

void ProgramHolder::stdOutPipeRunner()
{
	GetExitCodeProcess(processInformation.hProcess, &exitCode);
	DWORD numberOfBytesRead;
	BOOL result = true;
	while (exitCode == STILL_ACTIVE)
	{		
		char buffer[BUFFERSIZE] = { 0 };
		result = PeekNamedPipe(childStdOutRead, buffer, BUFFERSIZE, nullptr, &numberOfBytesRead, nullptr);
		if (numberOfBytesRead > 0)
		{
			result = ReadFile(childStdOutRead, buffer, BUFFERSIZE, &numberOfBytesRead, nullptr);
		}
		stdOut += std::string(buffer);
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		GetExitCodeProcess(processInformation.hProcess, &exitCode);
	}
	char buffer[BUFFERSIZE] = { 0 };
	while (!result && numberOfBytesRead != 0)
	{
		PeekNamedPipe(childStdOutRead, buffer, BUFFERSIZE, nullptr, &numberOfBytesRead, nullptr);
		if (numberOfBytesRead > 0)
		{
			result = ReadFile(childStdOutRead, buffer, BUFFERSIZE, &numberOfBytesRead, nullptr);
		}
	}
	running = false;
	Logger::log(className, __func__, InfoLevel::INFO, "Process Ended. Return Code = " + std::to_string(exitCode));
}

void ProgramHolder::clearStdOut()
{
	stdOut.clear();
}
