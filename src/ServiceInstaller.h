#pragma once
#include <string>
#include <Windows.h>

class ServiceInstaller
{
public:
	~ServiceInstaller();
	static unsigned long install();
	static void stopService(SC_HANDLE service, SERVICE_STATUS serviceStatus);
	static unsigned long remove();
private:
	ServiceInstaller();
	static unsigned long ServiceInstaller::cleanupAndReturn(SC_HANDLE& scManager, SC_HANDLE& service, std::string errorDescription, bool succeed);
	static ServiceInstaller *getInstance();
	static std::wstring serviceName,
	displayName,
	path;
};

