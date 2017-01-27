#pragma once
#include <string>
#include <Windows.h>

class WindowsRegistry
{
public:
    WindowsRegistry();
    static std::wstring getData(HKEY root, std::wstring key, std::wstring name);
    ~WindowsRegistry();
private:
    

};

