#include "WindowsUtilities.h"
#include <Windows.h>
#include <codecvt>
#include <vector>
#include <bitset>
#include <array>

WindowsUtilities::WindowsUtilities()
{
}


WindowsUtilities::~WindowsUtilities()
{
}

class WindowsUtilities::InstructionSet_Internal
{
    //TODO verify license(MLPL)
    //https://msdn.microsoft.com/en-us/library/hskdteyh(v=vs.140).aspx
public:
    InstructionSet_Internal()
        : nIds_{ 0 },
        nExIds_{ 0 },
        isIntel_{ false },
        isAMD_{ false },
        f_1_ECX_{ 0 },
        f_1_EDX_{ 0 },
        f_7_EBX_{ 0 },
        f_7_ECX_{ 0 },
        f_81_ECX_{ 0 },
        f_81_EDX_{ 0 },
        data_{},
        extdata_{}
    {
        //int cpuInfo[4] = {-1};  
        std::array<int, 4> cpui;

        // Calling __cpuid with 0x0 as the function_id argument  
        // gets the number of the highest valid function ID.  
        __cpuid(cpui.data(), 0);
        nIds_ = cpui[0];

        for (int i = 0; i <= nIds_; ++i)
        {
            __cpuidex(cpui.data(), i, 0);
            data_.push_back(cpui);
        }

        // Capture vendor string  
        char vendor[0x20];
        memset(vendor, 0, sizeof(vendor));
        *reinterpret_cast<int*>(vendor) = data_[0][1];
        *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
        *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
        vendor_ = vendor;
        if (vendor_ == "GenuineIntel")
        {
            isIntel_ = true;
        }
        else if (vendor_ == "AuthenticAMD")
        {
            isAMD_ = true;
        }

        // load bitset with flags for function 0x00000001  
        if (nIds_ >= 1)
        {
            f_1_ECX_ = data_[1][2];
            f_1_EDX_ = data_[1][3];
        }

        // load bitset with flags for function 0x00000007  
        if (nIds_ >= 7)
        {
            f_7_EBX_ = data_[7][1];
            f_7_ECX_ = data_[7][2];
        }

        // Calling __cpuid with 0x80000000 as the function_id argument  
        // gets the number of the highest valid extended ID.  
        __cpuid(cpui.data(), 0x80000000);
        nExIds_ = cpui[0];

        char brand[0x40];
        memset(brand, 0, sizeof(brand));

        for (int i = 0x80000000; i <= nExIds_; ++i)
        {
            __cpuidex(cpui.data(), i, 0);
            extdata_.push_back(cpui);
        }

        // load bitset with flags for function 0x80000001  
        if (nExIds_ >= 0x80000001)
        {
            f_81_ECX_ = extdata_[1][2];
            f_81_EDX_ = extdata_[1][3];
        }

        // Interpret CPU brand string if reported  
        if (nExIds_ >= 0x80000004)
        {
            memcpy(brand, extdata_[2].data(), sizeof(cpui));
            memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
            memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
            brand_ = brand;
        }
    };
    int nIds_;
    int nExIds_;
    std::string vendor_;
    std::string brand_;
    bool isIntel_;
    bool isAMD_;
    std::bitset<32> f_1_ECX_;
    std::bitset<32> f_1_EDX_;
    std::bitset<32> f_7_EBX_;
    std::bitset<32> f_7_ECX_;
    std::bitset<32> f_81_ECX_;
    std::bitset<32> f_81_EDX_;
    std::vector<std::array<int, 4>> data_;
    std::vector<std::array<int, 4>> extdata_;
};

std::wstring WindowsUtilities::getHomePath()
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
    InstructionSet_Internal CPUData;
    if(CPUData.f_1_ECX_[5])
    {
        return L"true";
    }
    return L"false";
}

