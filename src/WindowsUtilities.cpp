#include "WindowsUtilities.h"
#include <Windows.h>
#include <codecvt>
#include <vector>
#include <bitset>
#include <array>
#include "include/VirtualBox.h"
#include "VBoxMainController.h"

WindowsUtilities::WindowsUtilities()
{
}


WindowsUtilities::~WindowsUtilities()
{
}

std::wstring WindowsUtilities::getHomePathWstring()
{
	auto homePathDataBuffer(LPWSTR(malloc(256 * sizeof(WCHAR)))),
		homeDriveDataBuffer(LPWSTR(malloc(256 * sizeof(WCHAR))));
	GetEnvironmentVariableW(L"HOMEPATH", homePathDataBuffer, 255);
	GetEnvironmentVariableW(L"HOMEDRIVE", homeDriveDataBuffer, 255);
	auto homePath(std::wstring(homeDriveDataBuffer) + std::wstring(homePathDataBuffer));
	return homePath;
}

BSTR WindowsUtilities::toBSTR(std::wstring in)
{
	auto out(SysAllocStringLen(in.data(), in.size()));
	return out;
}

std::wstring WindowsUtilities::toWstring(BSTR in)
{
	return std::wstring(in, SysStringLen(in));
}

std::wstring WindowsUtilities::toWstring(std::string in)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	auto wstring(converter.from_bytes(in));
	return wstring;
}

std::wstring WindowsUtilities::getRAMSize()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return std::to_wstring(static_cast<int>(statex.ullTotalPhys / (1024 * 1024)));
}

std::wstring WindowsUtilities::getVMXSupport()
{
	auto iHost = VBoxMainController::getExistController()->getVirtualBoxIHost();
	int supported;
	iHost->GetProcessorFeature(ProcessorFeature_HWVirtEx, &supported);
	if (supported == TRUE)
	{
		return L"true";
	}
	return L"false";
}

void WindowsUtilities::ipconfigAndSave()
{
	system("ipconfig > C:\\SERVANT\\ipconfig.txt");
}
