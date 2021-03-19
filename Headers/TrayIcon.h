#pragma once

#include <Windows.h>
#include "AutoCast.h"

#define ID_ABOUT       2000
#define ID_EXIT        2001
#define ID_UPDATE      2002
#define ID_STARTONBOOT 2003
#define ID_TEXT 2004

BOOL ShowPopupMenu(HWND hWnd, bool checked) noexcept
{
	HMENU hPop = CreatePopupMenu();

	InsertMenuA(hPop, 0, MF_BYPOSITION | MF_GRAYED | MF_DISABLED, ID_TEXT, "Better TaskBar");
	InsertMenuA(hPop, 1, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
	InsertMenuA(hPop, 2, MF_BYPOSITION | (checked ? MF_CHECKED : MF_UNCHECKED), ID_STARTONBOOT, "Start On Boot");
	InsertMenuA(hPop, 3, MF_BYPOSITION | MF_STRING, ID_UPDATE, "Check For Updates");
	InsertMenuA(hPop, 4, MF_BYPOSITION | MF_STRING, ID_ABOUT, "About");
	InsertMenuA(hPop, 5, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");

	//SetMenuDefaultItem(hPop, ID_ABOUT, FALSE);
	SendMessage(hWnd, WM_INITMENUPOPUP, AutoCast(hPop).ToAuto<WPARAM>(), 0);

	POINT pt;
	GetCursorPos(&pt);
	const int cmd = TrackPopupMenu(hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd, NULL);
	SendMessage(hWnd, WM_COMMAND, cmd, 0);

	DestroyMenu(hPop);

	return 0;
}

void RemoveTrayIcon(HWND hWnd, UINT uID) noexcept
{
	NOTIFYICONDATA nid;
	nid.hWnd = hWnd;
	nid.uID = uID;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void AddTrayIcon(HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon) noexcept
{
	NOTIFYICONDATAA  nid;
	nid.hWnd = hWnd;
	nid.uID = uID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = uCallbackMsg;
	nid.hIcon = static_cast<HICON>(LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED));
	strncpy_s(nid.szTip, "Tool Tip", 128);
	Shell_NotifyIconA(NIM_ADD, &nid);
}

void ShowNotification(const char* title, const char* text, HICON ico, HWND window) noexcept
{
    NOTIFYICONDATAA nid;
    memset(&nid, 0, sizeof nid);
    nid.cbSize = sizeof nid;
    nid.uFlags = NIF_STATE | NIF_ICON | NIF_INFO;
    nid.dwInfoFlags = NIIF_USER;
	nid.hIcon = ico;
	nid.hWnd = window;
	strncpy_s(nid.szInfoTitle, title, 64);
	strncpy_s(nid.szInfo, text, 256);

    Shell_NotifyIconA(NIM_ADD, &nid);
}