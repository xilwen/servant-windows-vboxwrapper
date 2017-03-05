#pragma once
#pragma comment(lib, "mpr.lib")

#include <Windows.h>
#include <winnetwk.h>
#include <string>
#include <iostream>

class SambaFolderLinker
{
public :
	int connecting(LPNETRESOURCE netResource, LPCTSTR password, LPCTSTR userName);
	int disconnecting(LPCTSTR lpName, DWORD dwflags, BOOL ifForce);

	void setNetResource(LPSTR localName, LPSTR remoteName);
	NETRESOURCE getNetResource();

private:
	NETRESOURCE nr; 
	DWORD connectTemporary = CONNECT_TEMPORARY;
};
 