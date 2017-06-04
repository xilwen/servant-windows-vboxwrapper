#pragma once

#include "include/VirtualBox.h"
#include <Windows.h>
#include <vector>
#include "VirtualMachine.h"
#include "VBoxAppliance.h"

class VirtualMachine;

class VBoxMainController
{
public:
    VBoxMainController();
    static bool exist();
    std::wstring getVBoxVersion() const;
    VirtualMachine* getVMByNameOrUUID(std::wstring toSearch);
    static VBoxMainController* getInstance();
    VBoxAppliance* appliance();    
    static void refreshCache();
    IHost *getVirtualBoxIHost();
    ~VBoxMainController();
private:
    static bool initialized;
    
    //VirtualBox Stuff
    HRESULT lastResult;
    IVirtualBoxClient *virtualBoxClient;
    IVirtualBox *virtualBox;

    //VirtualBox Wrapper Handler
    std::vector<VirtualMachine*> machinesCache;
    VBoxAppliance *vboxAppliance;
    static VBoxMainController* activeInstance;
    
    //Initialization
    void init();
    void createVBoxClientInstance();
    void createVBoxInstance();
    std::wstring vBoxInstallDir; 

    void invalidateCache();
};

