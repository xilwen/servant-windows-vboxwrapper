#include "ServiceInstaller.h"

#include <sddl.h>
#include "Logger.h"
#include "WindowsUtilities.h"

std::wstring ServiceInstaller::serviceName(L"SERVANTVboxWrapper");
std::wstring ServiceInstaller::displayName(L"SERVANT VirtualBox Wrapper");
std::wstring ServiceInstaller::path(WindowsUtilities::getHomePathWstring() + L"\\SERVANT\\vBoxWrapper.exe");

ServiceInstaller::ServiceInstaller()
{
}

ServiceInstaller* ServiceInstaller::getInstance()
{
	static ServiceInstaller serviceInstaller;
	return &serviceInstaller;
}

ServiceInstaller::~ServiceInstaller()
{
}

unsigned long ServiceInstaller::cleanupAndReturn(SC_HANDLE& scManager, SC_HANDLE& service, std::string logDescription, bool succeed)
{
	if (scManager)
	{
		CloseServiceHandle(scManager);
		scManager = nullptr;
	}
	if (service)
	{
		CloseServiceHandle(service);
		service = nullptr;
	}
	auto errorCode = GetLastError();
	if (succeed)
	{
		Logger::log("ServiceInstaller", __func__, InfoLevel::INFO, logDescription + ", errorCode = " + std::to_string(errorCode));
	}
	else
	{
		Logger::log("ServiceInstaller", __func__, InfoLevel::ERR, logDescription + ", errorCode = " + std::to_string(errorCode));
	}
	return errorCode;
}

unsigned long ServiceInstaller::install()
{
	SC_HANDLE scManager = nullptr;
	SC_HANDLE service = nullptr;

	scManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (!scManager)
	{
		return cleanupAndReturn(scManager, service, "Can not open SCManager", false);
	}

	service = CreateServiceW(scManager, serviceName.c_str(), displayName.c_str(),
		SERVICE_QUERY_STATUS | WRITE_DAC, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL, path.c_str(), nullptr, nullptr, L"",
		L"LocalSystem", nullptr);
	if (!service)
	{
		return cleanupAndReturn(scManager, service, "Can not create service", false);
	}

	wchar_t stringSecurityDescriptor[] = L"D:" L"(A;;CCLCSWRPWPDTLOCRRC;;;SY)" L"(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;BA)"
		L"(A;;CCLCSWLOCRRC;;;AU)" L"(A;;CCLCSWRPWPDTLOCRRC;;;PU)" L"(A;;RPWP;;;IU)";
	PSECURITY_DESCRIPTOR psecurityDescriptor;
	if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(stringSecurityDescriptor, SDDL_REVISION_1, &psecurityDescriptor, nullptr))
	{
		return cleanupAndReturn(scManager, service, "Internal Error(ConvertStringSecurityDescriptorToSecurityDescriptorW)", false);
	}
	if (!SetServiceObjectSecurity(service, DACL_SECURITY_INFORMATION, psecurityDescriptor))
	{
		return cleanupAndReturn(scManager, service, "Set Service Security failed", false);
	}

	return cleanupAndReturn(scManager, service, "Service Installation succeed", true);
}

void ServiceInstaller::stopService(SC_HANDLE service, SERVICE_STATUS serviceStatus)
{
	if(ControlService(service, SERVICE_CONTROL_STOP, &serviceStatus))
	{
		Logger::log("ServiceInstaller", __func__, InfoLevel::INFO, "Stopping service...");
		Sleep(500);
		while(QueryServiceStatus(service, &serviceStatus) && serviceStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{
			Sleep(500);
		}
		if(serviceStatus.dwCurrentState == SERVICE_STOPPED)
		{
			Logger::log("ServiceInstaller", __func__, InfoLevel::INFO, "Service Stopped.");
		}
		else
		{
			Logger::log("ServiceInstaller", __func__, InfoLevel::ERR, "Can not Stop Service.");
		}
	}
}

unsigned long ServiceInstaller::remove()
{
	SC_HANDLE scManager = nullptr;
	SC_HANDLE service = nullptr;
	SERVICE_STATUS serviceStatus = {};

	scManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
	if(!scManager)
	{
		return cleanupAndReturn(scManager, service, "Can not open SCManager", false);
	}

	service = OpenServiceW(scManager, serviceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
	if(!service)
	{
		return cleanupAndReturn(scManager, service, "Can not open service", false);
	}

	stopService(service, serviceStatus);

	if(!DeleteService(service))
	{
		return cleanupAndReturn(scManager, service, "Can not delete service", false);
	}

	return cleanupAndReturn(scManager, service, "Service removed", true);
}

std::wstring ServiceInstaller::getServiceName()
{
	return serviceName;
}


