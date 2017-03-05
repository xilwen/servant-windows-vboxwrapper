#include "SambaFolderLinker.h"

int SambaFolderLinker::Connect(std::string serverName, std::string domainName, std::string userName, std::string password, std::string driverLetter)
{
	std::string strUserName;
	if (userName == "")
		strUserName = (domainName + "\\" + "default");
	else
		strUserName = (domainName + "\\" + userName);
	LPCTSTR lpUserName = (strUserName.c_str());
	std::string strServerName = ("\\\\" + domainName + "\\" + serverName);
	LPSTR netResourceServerName = const_cast<char *>(strServerName.c_str());
	
	LPCTSTR lpPassword = (password.c_str());
	std::string strDriverLetter = (driverLetter + ":");
	LPSTR lpDriverLetter = const_cast<char *>(strDriverLetter.c_str());

	setNetResource(lpDriverLetter, netResourceServerName);
	int	returnValue = WNetAddConnection2(&nr, lpPassword, lpUserName, DWFlags);
	return returnValue;
}

int SambaFolderLinker::Disconnect(std::string driverLetter, DWORD dwFlags, BOOL ifForce)
{
	int returnValue;
	std::string strDriverLetter = (driverLetter + ":");
	LPCTSTR lpDriverLetter = (strDriverLetter.c_str());
	if (dwFlags == CONNECT_UPDATE_PROFILE)
		returnValue = WNetCancelConnection2(lpDriverLetter, dwFlags, ifForce);
	else
		returnValue = WNetCancelConnection2(lpDriverLetter, 0, ifForce);

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

void SambaFolderLinker::setDWFlags(DWORD dwFlags)
{
	DWFlags = dwFlags;
}

DWORD SambaFolderLinker::getDWFlags()
{
	return DWFlags;
}