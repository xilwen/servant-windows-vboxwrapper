#pragma once

#include "VBoxMainController.h"
#include <stdexcept>
#include <iostream>

class VirtualMachine
{
public:
    VirtualMachine() = delete;
    explicit VirtualMachine(IMachine *machine);
    std::wstring getName();
    std::wstring getUuid();
    void setName(std::wstring newName);
    static void deleteMachineFully(std::wstring uuidOrName);
    void launch();
    void powerDown();
    void powerButton();
    void showScreenInVBoxUI() const;
    std::wstring getMachineStatus() const;
    ~VirtualMachine();
private:
    IMachine *machine;
    ISession *session = nullptr;
    IConsole *console = nullptr;
    IProgress *progress = nullptr;
    HRESULT lastResult;
};

