#pragma once

#include <Windows.h>
#include <string>
#include "AutoCast.h"

void AddAppToStartUp(std::string appname)
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, static_cast<char*>(buffer), sizeof(buffer));
	std::string progPath = static_cast<char*>(buffer);
	HKEY hkey = NULL;
	RegOpenKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
	RegSetValueExA(hkey, appname.c_str(), 0, REG_SZ, AutoCast(progPath.c_str()),
		AutoCast(((progPath.size() + 1) * sizeof(wchar_t))).ToAuto<DWORD>());
	RegCloseKey(hkey);
}

void RemoveAppFromStartUp(std::string appName) noexcept
{
	HKEY hkey = NULL;
	RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
	RegDeleteValueA(hkey, appName.c_str());
	RegCloseKey(hkey);
}


