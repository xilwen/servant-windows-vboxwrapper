#pragma once
#include <string>
#include <thread>

class VBoxAppliance
{
public:
    VBoxAppliance() = delete;
    explicit VBoxAppliance(const std::wstring& vBoxInstallDir);
    void import(const std::wstring& path);
	void exportOVA(std::wstring uuid, std::wstring path);
    static bool isBusy();
    static int getProgress();
    static bool isSucceeded();
    static std::wstring getNewVMName();
    ~VBoxAppliance();
private:
    std::wstring vBoxInstallDir,
        vBoxManagePath;
    static const std::string className;
    static FILE *consoleOutputPipe;
    static wchar_t buffer[128];
    static int progress;
    static bool succeeded;
    static bool busy;
	static bool exporting;
    static std::wstring suggestedName;
    std::wstring outputString;
    void childOutputHandler();
};

