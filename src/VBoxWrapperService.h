#pragma once
#include "Server.h"
#include "Interpreter.h"
#include "VBoxMainController.h"
#include <Windows.h>

class VBoxWrapperService
{
public:
	static VBoxWrapperService *getInstance();
	~VBoxWrapperService();
	static BOOL start();
	static bool isStopping();
private:
	VBoxWrapperService();

	wchar_t* serviceName;
	SERVICE_STATUS serviceStatus;
	SERVICE_STATUS_HANDLE serviceStatusHandle;
	BOOL stopping;
	HANDLE stoppedEvent;
	static void WINAPI serviceMain(DWORD argc, LPWSTR *argv);
	static void WINAPI serviceControlHandler(DWORD control);
	static void SetServiceStatus(DWORD currentState, DWORD error = NO_ERROR);
	static void stop();
	static void shutdown();
	void serviceWorkerThread();
	VBoxMainController *vboxMainController = nullptr;
	Interpreter *interpreter = nullptr;
	Server *server = nullptr;

};

