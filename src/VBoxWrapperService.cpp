#include "VBoxWrapperService.h"
#include "ServiceInstaller.h"
#include "Logger.h"

VBoxWrapperService::VBoxWrapperService()
{
	serviceName = new wchar_t[ServiceInstaller::getServiceName().size() + 1];
	wcsncpy(serviceName, ServiceInstaller::getServiceName().c_str(), ServiceInstaller::getServiceName().size());
	serviceStatusHandle = nullptr;
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;
	stopping = false;
	stoppedEvent = CreateEvent(nullptr, true, false, nullptr);
	if (!stoppedEvent)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "stoppedEvent cannot be created, errorCode=" + GetLastError());
		exit(EXIT_FAILURE);
	}
}

void VBoxWrapperService::serviceMain(DWORD argc, LPWSTR* argv)
{
	getInstance()->serviceStatusHandle = RegisterServiceCtrlHandlerW(getInstance()->serviceName, serviceControlHandler);
	if (!getInstance()->serviceStatusHandle)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "stoppedEvent cannot be created, errorCode=" + GetLastError());
		exit(EXIT_FAILURE);
	}

	try
	{
		SetServiceStatus(SERVICE_START_PENDING);
		getInstance()->vboxMainController = new VBoxMainController();
		getInstance()->interpreter = new Interpreter(getInstance()->vboxMainController);
		getInstance()->server = new Server(getInstance()->interpreter);
		SetServiceStatus(SERVICE_RUNNING);
		std::thread(&VBoxWrapperService::serviceWorkerThread, getInstance()).join();
	}
	catch (DWORD error)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "service start failed, errorCode=" + error);
		SetServiceStatus(SERVICE_STOPPED, error);
	}
	catch (std::exception &error)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, std::string("service start failed, error=") + error.what());
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (...)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "service start failed, error unknown.");
		SetServiceStatus(SERVICE_STOPPED);
	}
}

void VBoxWrapperService::serviceControlHandler(DWORD control)
{
	switch (control)
	{
	case SERVICE_CONTROL_STOP:
		stop();
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		shutdown();
		break;
	default:
		break;
	}
}

void VBoxWrapperService::SetServiceStatus(DWORD currentState, DWORD error)
{
	static DWORD checkPoint = 1;
	getInstance()->serviceStatus.dwCurrentState = currentState;
	getInstance()->serviceStatus.dwCheckPoint = (currentState == SERVICE_RUNNING || currentState == SERVICE_STOPPED) ? 0 : checkPoint++;
	getInstance()->serviceStatus.dwWin32ExitCode = NO_ERROR;
	getInstance()->serviceStatus.dwWaitHint = 0;
	::SetServiceStatus(getInstance()->serviceStatusHandle, &getInstance()->serviceStatus);
}

void VBoxWrapperService::stop()
{
	auto originalState = getInstance()->serviceStatus.dwCurrentState;
	try
	{
		SetServiceStatus(SERVICE_STOP_PENDING);
		Logger::log("VBoxWrapperService", __func__, InfoLevel::INFO, "stopping service...");
		if (getInstance()->server)
			delete getInstance()->server;
		if (getInstance()->interpreter)
			delete getInstance()->interpreter;
		if (getInstance()->vboxMainController)
			delete getInstance()->vboxMainController;
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (DWORD error)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "service stop failed, errorCode=" + error);
		SetServiceStatus(originalState);
	}
	catch (...)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "service stop failed, error unknown.");
		SetServiceStatus(originalState);
	}
}

void VBoxWrapperService::shutdown()
{
	try
	{
		if (getInstance()->server)
			delete getInstance()->server;
		if (getInstance()->interpreter)
			delete getInstance()->interpreter;
		if (getInstance()->vboxMainController)
			delete getInstance()->vboxMainController;
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (DWORD error)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "service shutdown failed, errorCode=" + error);
	}
	catch (...)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, "service shutdown failed, error unknown.");
	}
}

void VBoxWrapperService::serviceWorkerThread()
{
	try
	{
		server->waitForConnect();
		server->handShakeWithClient();
		server->runInterpreterDaemon();
	}
	catch (std::exception &error)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, std::string("Something happend, ") + error.what());
	}
	catch (...)
	{
		Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, std::string("Something is out of control..."));
	}

	SetEvent(stoppedEvent);
}

BOOL VBoxWrapperService::start()
{
	SERVICE_TABLE_ENTRYW serviceTableEntry[] = {
		{getInstance()->serviceName, serviceMain}, {nullptr, nullptr}
	};
	return StartServiceCtrlDispatcherW(serviceTableEntry);
}

bool VBoxWrapperService::isStopping()
{
	return static_cast<bool>(getInstance()->stopping);
}

VBoxWrapperService* VBoxWrapperService::getInstance()
{
	static VBoxWrapperService vboxWrapperService;
	return &vboxWrapperService;
}

VBoxWrapperService::~VBoxWrapperService()
{
	if (serviceName)
	{
		delete serviceName;
	}
	Logger::log("VBoxWrapperService", __func__, InfoLevel::ERR, std::string("Service destructing"));

	if (getInstance()->server)
		delete getInstance()->server;
	if (getInstance()->interpreter)
		delete getInstance()->interpreter;
	if (getInstance()->vboxMainController)
		delete getInstance()->vboxMainController;
}


