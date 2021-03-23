#pragma once

#include <UIAutomation.h>
#include <atlcomcli.h>
#include "AutoCast.h"
#include <iostream>

#define SafeRelease(__) if (__) { __.Release(); __ = NULL; }

enum class AccentState
{
	ACCENT_DISABLED = 0,
	ACCENT_ENABLE_GRADIENT = 1,
	ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
	ACCENT_ENABLE_BLURBEHIND = 3,
	ACCENT_INVALID_STATE = 4
};

void SetWindowBlur(HWND hWnd, AccentState AccentState, int Flags = 2, int color = 0, int AnimationId = 0) noexcept
{
	HINSTANCE hlib = LoadLibraryW(L"User32.dll");

	if (hlib)
	{
		struct ACCENTPOLICY { int nAccentState; int nFlags; int nColor; int nAnimationId; };
		struct WINCOMPATTRDATA { int nAttribute; void* pData; unsigned long ulDataSize; };

		typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
		static pSetWindowCompositionAttribute SetWindowCompositionAttribute = AutoCast<FARPROC>(GetProcAddress(hlib, "SetWindowCompositionAttribute"));

		if (SetWindowCompositionAttribute)
		{
			ACCENTPOLICY policy = { static_cast<int>(AccentState), Flags, color, AnimationId };
			WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) };
			SetWindowCompositionAttribute(hWnd, &data);
		}

		FreeLibrary(hlib);
	}
}

void CenterTaskBar(int offset, bool def = false)
{
	//const int result = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	HWND taskbar = FindWindowA("Shell_TrayWnd", NULL);
	HWND panel = FindWindowExA(taskbar, NULL, "ReBarWindow32", NULL);
	HWND panel2 = FindWindowExA(panel, NULL, "MSTaskSwWClass", NULL);
	HWND tasklist = FindWindowExA(panel2, NULL, "MSTaskListWClass", NULL);

	RECT rc{}, rc2{}, rc3{}, rc4{}, rc5{}, rc6{}, rc7{};

	CComPtr<IUIAutomation> UI{};
	CComPtr<IUIAutomationTreeWalker> TreeWalker{};
	CComPtr<IUIAutomationElement> firstchild{};
	CComPtr<IUIAutomationElement> lastchild{};
	CComPtr<IUIAutomationElement> taskList{};

	HRESULT hr;
	hr = ::CoInitialize(NULL);
	hr = ::CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), AutoCast<LPVOID>(&UI));

	UI->ElementFromHandle(tasklist, &taskList);
	UI->get_ControlViewWalker(&TreeWalker);

	TreeWalker->GetFirstChildElement(taskList, &firstchild);
	TreeWalker->GetLastChildElement(taskList, &lastchild);

	firstchild->get_CurrentBoundingRectangle(&rc2);
	lastchild->get_CurrentBoundingRectangle(&rc3);

	GetWindowRect(tasklist, &rc);
	GetWindowRect(panel2, &rc4); // parent
	GetClientRect(tasklist, &rc5);
	GetClientRect(taskbar, &rc6);
	GetWindowRect(panel, &rc7);

	const long offset1 = rc2.right - rc2.left;
	const long Center = (rc5.right - rc5.left) - ((((rc3.left - rc2.left)) + offset1) / 2) - (rc4.left + (((rc5.left - rc.left) + offset1) - ((rc5.left - rc2.left))));

	SafeRelease(UI);
	SafeRelease(TreeWalker);
	SafeRelease(firstchild);
	SafeRelease(lastchild);
	SafeRelease(taskList);

	::CoUninitialize();

	if (rc6.right > rc6.bottom)
		::SetWindowPos(tasklist, nullptr, def ? offset : Center + offset, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_ASYNCWINDOWPOS);
	else
		::SetWindowPos(tasklist, nullptr, 0, def ? offset : Center + offset, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_ASYNCWINDOWPOS);
}
	

void SetTaskBarColor(HWND hWnd, COLORREF color, BYTE Alpha) noexcept
{
	SetWindowBlur(hWnd, AccentState::ACCENT_ENABLE_GRADIENT, 0, color);
}

void CreateAnimation(int offset_L, int offset_R, int offset, int waitms_L = 0, int waitms_R = 0, int loop = 1, bool defult = false)
{
	for (int i = 0; i < loop; i++)
	{
		for (int j = 0; j < offset_L; j++)
		{
			Sleep(waitms_L);
			CenterTaskBar(j + offset, defult);
		}

		for (int j = offset_R; j >= 0; j--)
		{
			Sleep(waitms_R);
			CenterTaskBar(j + offset, defult);
		}
	}
}