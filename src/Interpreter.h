#pragma once
#include <string>
#include <queue>
#include "VBoxMainController.h"
#include "VirtualMachine.h"

class Interpreter
{
public:
    Interpreter() = delete;
    explicit Interpreter(VBoxMainController* vbmc);
    ~Interpreter();
    std::wstring run(std::wstring wstring);
    void reset();
private:
    std::queue<std::wstring> *InterpretQueue = nullptr;
    std::wstring import();
    std::wstring set();
    std::wstring del();
    std::wstring interpret();
    std::wstring popInterpretQueue() const;
    VirtualMachine *vmSelected = nullptr;
    static std::queue<std::wstring>* makeCommandQueue(const std::wstring& string);
    VBoxMainController* vbmc;

    std::wstring search();
    std::wstring control();
    std::wstring get();
    static std::wstring error(std::wstring what);
};

