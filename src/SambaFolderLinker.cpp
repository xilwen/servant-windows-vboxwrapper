#include "SambaFolderLinker.h"

int SambaFolderLinker::connecting(LPNETRESOURCE netResource, LPCTSTR password, LPCTSTR username)
{
	int	returnValue = WNetAddConnection2(netResource, password, username, connectTemporary);
	return returnValue;
}

int SambaFolderLinker::disconnecting(LPCTSTR lpName, DWORD dwflags, BOOL ifForce)
{
	int returnValue = WNetCancelConnection2(lpName, dwflags, ifForce);
	return returnValue;
}

void SambaFolderLinker::setNetResource(LPSTR localName, LPSTR remoteName)
{
	nr.dwType = RESOURCETYPE_DISK;
	nr.lpLocalName = localName;
	nr.lpRemoteName = remoteName;
	nr.lpProvider = NULL;
}

NETRESOURCE SambaFolderLinker::getNetResource()
{
	return nr;
}
