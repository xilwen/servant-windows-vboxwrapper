#include "WindowsRegistry.h"

WindowsRegistry::WindowsRegistry()
{

}

std::wstring WindowsRegistry::getData(HKEY root, std::wstring key, std::wstring dataName)
{
    HKEY hkey;
    DWORD type, data;

    //Check is 64bit Windows or not, if 64bit then use Wow64 flag to read registry
    //detection method base on https://msdn.microsoft.com/en-us/library/ms684139(v=vs.85).aspx
    typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    if(reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process")))
    {
        //64bit
        if (RegOpenKeyExW(root, key.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &hkey) != ERROR_SUCCESS)
        {
            throw std::runtime_error("Registry Key cannot be opened");
        }
    }
    else
    {
        //32bit
        if (RegOpenKeyExW(root, key.c_str(), 0, KEY_READ, &hkey) != ERROR_SUCCESS)
        {
            throw std::runtime_error("Registry Key cannot be opened");
        }
    }
    
    if (RegQueryValueExW(hkey, dataName.c_str(), nullptr, &type, nullptr, &data) != ERROR_SUCCESS)
    {
        RegCloseKey(hkey);
        throw std::runtime_error("Registry value cannot be read");
    }
    if (type != REG_SZ)
    {
        RegCloseKey(hkey);
        throw std::runtime_error("This registry is not REG_SZ(String) Type");
    }
    std::wstring wstring(data / sizeof(wchar_t), L'\0');
    if (RegQueryValueExW(hkey, dataName.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(&wstring[0]), &data))
    {
        RegCloseKey(hkey);
        throw std::runtime_error("Registry data cannot be read");
    }
    RegCloseKey(hkey);
    
    if(wstring.find_first_of(L'\0') != std::string::npos)
    {
        wstring.resize(wstring.find_first_of(L'\0'));
    }
    return wstring;
}


WindowsRegistry::~WindowsRegistry()
{
}
