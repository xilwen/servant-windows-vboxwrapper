#include "VBoxAppliance.h"
#include "Logger.h"
#include <cstdio>
#include <locale>
#include <codecvt>

bool VBoxAppliance::busy(false);
bool VBoxAppliance::succeeded(false);
std::wstring VBoxAppliance::suggestedName;
int VBoxAppliance::progress(0);
FILE *VBoxAppliance::consoleOutputPipe(nullptr);
wchar_t VBoxAppliance::buffer[128] = { L'\0' };
const std::string VBoxAppliance::className("VBoxAppliance");

VBoxAppliance::VBoxAppliance(const std::wstring& vBoxInstallDir) :vBoxInstallDir(vBoxInstallDir)
{
    if (this->vBoxInstallDir.back() != L'\\')
    {
        this->vBoxInstallDir.push_back(L'\\');
    }
    vBoxManagePath = vBoxInstallDir + L"VBoxManage.exe";
}

void VBoxAppliance::import(const std::wstring& path)
{
    if (busy)
    {
        throw std::runtime_error("VBoxAppliance is busy, try again later");
    }
    busy = true;
    succeeded = false;
    auto fullPath(L'\"' + vBoxManagePath + L'\"' + L" import " + path + L" 2>&1");
    if ((consoleOutputPipe = _wpopen(fullPath.c_str(), L"rt, ccs=UNICODE")) == nullptr)
    {
        throw std::runtime_error("Can not start VBoxManage process");
    }
    std::thread childWatchingThread(&VBoxAppliance::childOutputHandler, this);
    childWatchingThread.detach();
}

bool VBoxAppliance::isBusy()
{
    return busy;
}

int VBoxAppliance::getProgress()
{
    return progress;
}

bool VBoxAppliance::isSucceeded()
{
    return succeeded;
}

std::wstring VBoxAppliance::getNewVMName()
{
    return suggestedName;
}

VBoxAppliance::~VBoxAppliance()
{
}

void VBoxAppliance::childOutputHandler()
{
    auto firstPass(true);
    succeeded = false;
    outputString.clear();
    progress = 0;
    auto bufferLimit(20);
    for (auto i = 0; fgetws(buffer, bufferLimit, consoleOutputPipe); ++i)
    {
        if (i > 12)
        {
            bufferLimit = 127;
        }
        outputString += std::wstring(buffer);
        auto firstPartCompletePos(outputString.find(L"100%"));
        if (firstPartCompletePos != std::wstring::npos)
        {
            firstPass = false;
        }
        if (!firstPass)
        {
            if (outputString.substr(firstPartCompletePos + 4).find(L"100%") != std::wstring::npos)
                progress = 100;
            else if (outputString.substr(firstPartCompletePos + 4).find(L"80%") != std::wstring::npos)
                progress = 80;
            else if (outputString.substr(firstPartCompletePos + 4).find(L"60%") != std::wstring::npos)
                progress = 60;
            else if (outputString.substr(firstPartCompletePos + 4).find(L"40%") != std::wstring::npos)
                progress = 40;
            else if (outputString.substr(firstPartCompletePos + 4).find(L"20%") != std::wstring::npos)
                progress = 20;
        }
    }

    if (feof(consoleOutputPipe))
    {
        auto errorLevel = _pclose(consoleOutputPipe);
        if (errorLevel == 0)
        {
            succeeded = true;

            std::wstring searchNameKeyword(L"Suggested VM name \"");
            auto vmNamePos(outputString.find(searchNameKeyword));
            auto vmNamePosEnd(outputString.substr(vmNamePos + searchNameKeyword.size()).find(L"\""));
            suggestedName = outputString.substr(vmNamePos + searchNameKeyword.size(), vmNamePosEnd);

            Logger::log(className, __func__, InfoLevel::INFO,
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>().to_bytes(L"suggestedName is " + suggestedName));
        }
        Logger::log(className, __func__, InfoLevel::INFO, "VBoxManage returns " + std::to_string(errorLevel));
    }
    else
    {
        Logger::log(className, __func__, InfoLevel::ERR, "Strange Exception happened");
    }
    busy = false;
}
