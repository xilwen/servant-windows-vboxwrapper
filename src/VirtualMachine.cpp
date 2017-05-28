#include "VirtualMachine.h"
//TODO Refactor to use SysFreeString(BSTR);

VirtualMachine::VirtualMachine(IMachine *machine)
{
    if (!VBoxMainController::exist())
    {
        throw std::runtime_error("Must have a VBoxMainController exist to create a VirtualMachine class instance.");
    }
    if (!machine)
    {
        throw std::runtime_error("Can not create VirtualMachine instance from nullptr.");
    }
    this->machine = machine;
    lastResult = NULL;
}

std::wstring VirtualMachine::getName()
{
    BSTR str;
    lastResult = machine->get_Name(&str);
    if (SUCCEEDED(lastResult))
    {
        return std::wstring(str, SysStringLen(str));
    }
    else
    {
        std::cerr << "Can not get the machine's name!\n";
        return std::wstring();
    }
}

std::wstring VirtualMachine::getUuid()
{
    BSTR uuid;
    lastResult = machine->get_Id(&uuid);
    if (!SUCCEEDED(lastResult))
    {
        throw std::runtime_error("Error retrieving machine ID!");
    }
    return std::wstring(uuid, SysStringLen(uuid));
}

void VirtualMachine::setName(std::wstring newName)
{
    auto bstrName = SysAllocStringLen(newName.data(), newName.size());
    lastResult = machine->put_Name(bstrName);
}

void VirtualMachine::deleteMachineFully(std::wstring uuidOrName)
{
    auto toDelete = VBoxMainController::getExistController()->getVMByNameOrUUID(uuidOrName);
    if (!toDelete)
        return;
    SAFEARRAY *mediumArray;
    IProgress *iprogress;
    toDelete->machine->Unregister(CleanupMode_Full, &mediumArray);
    toDelete->machine->DeleteConfig(mediumArray, &iprogress);
    try
    {
        SafeArrayUnaccessData(mediumArray);
        SafeArrayDestroy(mediumArray);
    }
    catch(std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
    catch(...)
    {
        throw std::runtime_error("undefined exception happened");
    }
    //TODO Refactor use thread, but need a lock to prevent program shutdown.
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void VirtualMachine::launch()
{
    if (getMachineStatus() != L"PoweredOff")
    {
        throw std::runtime_error("VM did not powered off, launch request rejected.");
    }
    auto sessiontype = SysAllocString(L"headless");
    BSTR uuid;
    lastResult = machine->get_Id(&uuid);
    if (!SUCCEEDED(lastResult))
    {
        throw std::runtime_error("Error retrieving machine ID!");
    }
    lastResult = CoCreateInstance(CLSID_Session,        /* the VirtualBox base object */
        nullptr,                 /* no aggregation */
        CLSCTX_INPROC_SERVER, /* the object lives in the current process */
        IID_ISession,         /* IID of the interface */
        reinterpret_cast<void**>(&session));
    if (!SUCCEEDED(lastResult))
    {
        throw std::runtime_error("Error creating Session instance!");
    }
    lastResult = machine->LaunchVMProcess(session, sessiontype,
        nullptr, &progress);
    if (!SUCCEEDED(lastResult))
    {
        throw std::runtime_error("Could not open remote session!");
    }
    progress->WaitForCompletion(-1);
    session->get_Console(&console);
}

std::wstring VirtualMachine::getMachineStatus() const
{
    //String defined using SDKRef.pdf from SDK bundle, 6.65(Page 360).
    MachineState State;
    machine->get_State(&State);
    switch (State)
    {
        //TODO wrap more MachineState here
    case MachineState_PoweredOff:
        return L"PoweredOff";
    case MachineState_Running:
        return L"Running";
    case MachineState_Aborted:
        return L"Aborted";
    case MachineState_Null:
        return L"Null";
    case MachineState_Paused:
        return L"Paused";
    case MachineState_Starting:
        return L"Starting";
    case MachineState_Stopping:
        return L"Stopping";
    case MachineState_Snapshotting:
        return L"Snapshotting";
    case MachineState_DeletingSnapshot:
        return L"DeletingSnapshot";
    case MachineState_RestoringSnapshot:
        return L"RestoringSnapshot";
    default:
        return L"Unknown";
    }
}

void VirtualMachine::powerDown()
{
    if (getMachineStatus() != L"Running")
    {
        throw std::runtime_error("VM did not powered on, request rejected.");
    }
	if (!console || !session)
	{
		throw std::runtime_error("We do not have the handle of iconsole/isession");
	}
    console->PowerDown(&progress);
    progress->WaitForCompletion(-1);
    session->UnlockMachine();
}

void VirtualMachine::powerButton()
{
    if (getMachineStatus() != L"Running")
    {
        throw std::runtime_error("VM did not powered on, request rejected.");
    }
    //TODO incomplete function only for demo
	if (!console || !session)
	{
		throw std::runtime_error("We do not have the handle of iconsole/isession");
	}
    console->PowerButton();
    session->UnlockMachine();
}


void VirtualMachine::showScreenInVBoxUI() const
{
    //TODO Read VirtualBox source and find out how to attach GUI to a headless VM
    throw std::runtime_error("This function is not implemented!");
}


VirtualMachine::~VirtualMachine()
{
    if (session)
        session->Release();
    if (console)
        console->Release();
    if (progress)
        progress->Release();
    if (machine)
        machine->Release();
}
