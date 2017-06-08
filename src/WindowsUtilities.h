#pragma once
#include <string>
#include <Windows.h>


class WindowsUtilities
{
public:
    WindowsUtilities();
    ~WindowsUtilities();
    static std::wstring getHomePathWstring();
    static BSTR toBSTR(std::wstring in);
    static std::wstring toWstring(BSTR in);
    static std::wstring toWstring(std::string in);
    static std::wstring getRAMSize();
    static std::wstring getVMXSupport();
	static std::wstring getCPUCount();
	static void ipconfigAndSave();
private:
    std::wstring homePath;
};
