#pragma once
#pragma comment(lib, "mpr.lib")

#include <Windows.h>
#include <winnetwk.h>
#include <string>
#include <iostream>

class SambaFolderLinker
{
public :
	int Connect(std::string serverName, std::string domainName, std::string userName, std::string password, std::string oneCharDiskName);
	int Disconnect(std::string driverLetter, DWORD dwFlags, BOOL ifForce);

	void setNetResource(LPSTR localName, LPSTR remoteName);
	NETRESOURCE getNetResource();
	void setDWFlags(DWORD dwFlags);
	DWORD getDWFlags();

private:
	NETRESOURCE nr; 
	DWORD DWFlags = CONNECT_TEMPORARY;
	
};
 