#include "VBoxMainController.h"
#include "WindowsRegistry.h"
#include "Logger.h"
#include <stdexcept>
#include <sstream>

bool VBoxMainController::initialized(false);
VBoxMainController* VBoxMainController::activeInstance(nullptr);

void VBoxMainController::init()
{
    createVBoxClientInstance();
    createVBoxInstance();
}

VBoxMainController::VBoxMainController()
{
    if (exist())
    {
        throw std::runtime_error("Only one instance of VBoxMainController is allowed.");
    }
    activeInstance = this;
    CoInitialize(nullptr);
    init();
    try
    {
        vBoxInstallDir = WindowsRegistry::getData(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Oracle\\VirtualBox", L"InstallDir");
    }
    catch (...)
    {
        Logger::log("VBoxMainController", __func__, InfoLevel::WARNING, "Can not found installDir in Registry. Will try use default location.");
        vBoxInstallDir = L"C:\\Program Files\\Oracle\\VirtualBox\\";
    }
    vboxAppliance = new VBoxAppliance(vBoxInstallDir);
}

void VBoxMainController::createVBoxClientInstance()
{
    lastResult = CoCreateInstance(CLSID_VirtualBoxClient, /* the VirtualBoxClient object */
        nullptr,                   /* no aggregation */
        CLSCTX_INPROC_SERVER,   /* the object lives in the current process */
        IID_IVirtualBoxClient,  /* IID of the interface */
        reinterpret_cast<void**>(&virtualBoxClient));
    if (SUCCEEDED(lastResult))
    {
        initialized = true;
    }
    else
    {
        std::stringstream outputHexRC;
        outputHexRC << "Virtualbox instance creation failed, HRESULT = 0x" << std::hex << lastResult;
        throw std::runtime_error(outputHexRC.str());
    }
}

void VBoxMainController::createVBoxInstance()
{
    if (!virtualBoxClient)
    {
        throw std::runtime_error("IVirtualBoxClient needs to be created before VBox creation.");
    }
    lastResult = virtualBoxClient->get_VirtualBox(&virtualBox);
    if (!SUCCEEDED(lastResult))
    {
        throw std::runtime_error("Can not create IVirtualbox instance." + std::to_string(static_cast<unsigned long>(lastResult)));
    }
}

bool VBoxMainController::exist()
{
    return initialized;
}

std::wstring VBoxMainController::getVBoxVersion() const
{
    BSTR version;
    virtualBox->get_Version(&version);
    return std::wstring(version, SysStringLen(version));
}

VirtualMachine * VBoxMainController::getVMByNameOrUUID(std::wstring toSearch)
{
    //TODO cacher and cache invalidator
    //find if already cached
    for (auto i : machinesCache)
    {
        if (i->getName().find(toSearch) != std::wstring::npos ||
            i->getUuid().find(toSearch) != std::wstring::npos)
        {
            return i;
        }
    }
    
    //if not cached then create instance and return
    IMachine *machine;
    lastResult = virtualBox->FindMachine(BSTR(SysAllocStringLen(toSearch.data(), toSearch.size())), &machine);
    if(SUCCEEDED(lastResult))
    {
        if (!machine)
            throw std::range_error("Did not found the specified VM.");
        auto newCachedMachine = new VirtualMachine(machine);
        machinesCache.push_back(newCachedMachine);
        return newCachedMachine;
    }
    throw std::range_error("Did not found the specified VM.");
}


VBoxMainController * VBoxMainController::getInstance()
{
    return activeInstance;
}

VBoxAppliance* VBoxMainController::appliance()
{
    return vboxAppliance;
}

void VBoxMainController::refreshCache()
{
    getInstance()->invalidateCache();
}

IHost *VBoxMainController::getVirtualBoxIHost()
{
    IHost *iHost = nullptr;
    virtualBox->get_Host(&iHost);
    return iHost;
}

void VBoxMainController::invalidateCache()
{
	Logger::log("VBoxMainController", __func__, InfoLevel::INFO, "cache invalidation started");
    for (auto x : machinesCache)
    {
        if (x)
        {
            delete x;
        }
    }
    machinesCache.clear();
	Logger::log("VBoxMainController", __func__, InfoLevel::INFO, "cache invalidation ended");
}

VBoxMainController::~VBoxMainController()
{
    invalidateCache();
    virtualBox->Release();
    virtualBoxClient->Release();    
    CoUninitialize();
    activeInstance = nullptr;
    initialized = false;
}
