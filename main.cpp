#include "TrayIcon.h"
#include "Taskbar.h"
#include "Updater.h"
#include "IniFile.h"
#include "registry.h"
#include "AutoCast.h"

#include <Commctrl.h>
#include <ShellScalingAPI.h>
#include <iostream>
#include <thread>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
 processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define GUI_CREATEFONT(Height, Width, Font, hwnd) SendMessage(hwnd, WM_SETFONT, (LPARAM)(HFONT)CreateFontA(Height, Width, 0, 0, \
 FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, Font), TRUE);

std::string GetWinText(HWND W)
{
	char buff[1024] = { 0 };
	GetWindowTextA(W, static_cast<char*>(buff), 1024);
	std::string text = static_cast<char*>(buff);
	return text;
}

void WindowMessageLoop() noexcept
{
	MSG msg{};
	BOOL result{};

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		result = ::GetMessage(&msg, NULL, 0, 0);

		if (result == 0) break;
		else
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND h16 = NULL, h18 = NULL, h20 = NULL, h9 = NULL, h40 = NULL;
	static bool checked{};

	HWND taskbar = FindWindowA("Shell_TrayWnd", NULL);
	HWND startmenu = FindWindowA("Windows.UI.Core.CoreWindow", "Start");

	switch (msg)
	{
	case WM_CREATE:
	{
		AddTrayIcon(hwnd, 1, WM_APP, 0);

		HWND h1 = CreateWindow(TEXT("STATIC"), TEXT("TASKBAR STYLE"), WS_CHILD | WS_VISIBLE, 20, 30, 300, 25, hwnd, NULL, NULL, NULL);
		HWND h2 = CreateWindow(TEXT("button"), TEXT("None"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 20, 70, 200, 30, hwnd, (HMENU)5, NULL, NULL);
		HWND h3 = CreateWindow(TEXT("button"), TEXT("Gradient"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 100, 200, 30, hwnd, (HMENU)1, NULL, NULL);
		HWND h4 = CreateWindow(TEXT("button"), TEXT("Transparent"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 140, 230, 30, hwnd, (HMENU)2, NULL, NULL);
		HWND h5 = CreateWindow(TEXT("button"), TEXT("Blur"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 180, 200, 30, hwnd, (HMENU)3, NULL, NULL);
		HWND h6 = CreateWindow(TEXT("button"), TEXT("Acrylic"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 210, 200, 30, hwnd, (HMENU)6, NULL, NULL);
		
		HWND h100 = CreateWindow(TEXT("STATIC"), TEXT("STARTMENU"), WS_CHILD | WS_VISIBLE, 20, 270, 320, 30, hwnd, NULL, NULL, NULL);
		HWND h101 = CreateWindow(TEXT("button"), TEXT("None"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 20, 310, 200, 30, hwnd, (HMENU)102, NULL, NULL);
		HWND h103 = CreateWindow(TEXT("button"), TEXT("Blur"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 350, 200, 30, hwnd, (HMENU)103, NULL, NULL);

		HWND h11 = CreateWindow(TEXT("STATIC"), TEXT("TASKBAR POSITION"), WS_CHILD | WS_VISIBLE, 270, 30, 300, 25, hwnd, NULL, NULL, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("OFFSET"), WS_CHILD | WS_VISIBLE, 270, 85, 300, 25, hwnd, NULL, NULL, NULL);
		h9 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE, 400, 80, 80, 25, hwnd, AutoCast<int>(12), NULL, NULL);

		HWND h14 = CreateWindow(TEXT("STATIC"), TEXT("TASKBAR COLOR"), WS_CHILD | WS_VISIBLE, 270, 160, 300, 25, hwnd, NULL, NULL, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("RED"), WS_CHILD | WS_VISIBLE, 270, 200, 300, 25, hwnd, NULL, NULL, NULL);
		h16 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE, 400, 200, 80, 25, hwnd, AutoCast<int>(19), NULL, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("GREEN"), WS_CHILD | WS_VISIBLE, 270, 240, 300, 25, hwnd, NULL, NULL, NULL);
		h18 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE, 400, 240, 80, 25, hwnd, AutoCast<int>(20), NULL, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("BLUE"), WS_CHILD | WS_VISIBLE, 270, 285, 300, 25, hwnd, NULL, NULL, NULL);
		h20 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE, 400, 285, 80, 25, hwnd, AutoCast<int>(21), NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("RESET"), WS_CHILD | WS_VISIBLE, 280, 400, 100, 35, hwnd, (HMENU)10003, NULL, NULL);
		CreateWindow(TEXT("button"), TEXT("APPLY"), WS_CHILD | WS_VISIBLE, 400, 400, 100, 35, hwnd, (HMENU)10002, NULL, NULL);

		GUI_CREATEFONT(30, 12, "Arial", h11); GUI_CREATEFONT(30, 12, "Arial", h14); GUI_CREATEFONT(30, 12, "Arial", h1);
		GUI_CREATEFONT(30, 12, "Arial", h2); GUI_CREATEFONT(30, 12, "Arial", h3); GUI_CREATEFONT(30, 12, "Arial", h4);
		GUI_CREATEFONT(30, 12, "Arial", h5); GUI_CREATEFONT(30, 12, "Arial", h6); GUI_CREATEFONT(30, 12, "Arial", h100); 
		GUI_CREATEFONT(30, 12, "Arial", h101); GUI_CREATEFONT(30, 12, "Arial", h103);

		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 10002: 
		{
			if (IsDlgButtonChecked(hwnd, 10002) == BN_CLICKED)
			{
				while (true)
				{
					if (IsDlgButtonChecked(hwnd, 1) == BST_CHECKED) SetWindowBlur(taskbar, AccentState::ACCENT_ENABLE_GRADIENT);		
					if (IsDlgButtonChecked(hwnd, 2) == BST_CHECKED) SetWindowBlur(taskbar, AccentState::ACCENT_ENABLE_TRANSPARENTGRADIENT);		
					if (IsDlgButtonChecked(hwnd, 3) == BST_CHECKED) SetWindowBlur(taskbar, AccentState::ACCENT_ENABLE_BLURBEHIND);
					if (IsDlgButtonChecked(hwnd, 6) == BST_CHECKED) SetWindowBlur(taskbar, AccentState::ACCENT_INVALID_STATE);				
					if (IsDlgButtonChecked(hwnd, 5) == BST_CHECKED) SetWindowBlur(taskbar, AccentState::ACCENT_DISABLED);
			
					if (atoi(GetWinText(h16).c_str()) != 0 || atoi(GetWinText(h18).c_str()) != 0 || atoi(GetWinText(h20).c_str()) != 0)
						SetTaskBarColor(taskbar, RGB(atoi(GetWinText(h16).c_str()), atoi(GetWinText(h18).c_str()), atoi(GetWinText(h20).c_str())), NULL);

					if (IsDlgButtonChecked(hwnd, 102) == BST_CHECKED) SetWindowBlur(startmenu, AccentState::ACCENT_DISABLED);
					if (IsDlgButtonChecked(hwnd, 103) == BST_CHECKED) SetWindowBlur(startmenu, AccentState::ACCENT_ENABLE_BLURBEHIND);

					CenterTaskBar(atoi(GetWinText(h9).c_str()));

					Sleep(10);
					WindowMessageLoop();
				}
			}

			break;
		}
		
		case 10003: 
		{
			if (IsDlgButtonChecked(hwnd, 10003) == BN_CLICKED)
			{
				while (true)
				{
					Sleep(10);
					CenterTaskBar(0, true);
					WindowMessageLoop(); 
				}
			}

			break;
		}

		case ID_ABOUT:
			MessageBoxA(NULL, "About", "", MB_OK);
			break;
		case ID_UPDATE:
			if (MessageBoxA(NULL, "Updater Not Available at the Current Version of BetterTaskbar", "Update", MB_OK)) //MB_YESNOCANCEL == IDYES
			{
				std::cout << "Updateing" << std::endl;
				//Update("", "", "");
			}
			break;
		case ID_EXIT:
			RemoveTrayIcon(hwnd, 1);
			CenterTaskBar(0, true);
			exit(0);
			break;
		case ID_STARTONBOOT:
			if (checked)
			{
				checked = false;
				RemoveAppFromStartUp("BetterTaskBar");
				InIFileSetValue("Settings", "StartUp", "false", std::ios::ate);
			}
			else
			{
				checked = true;
				AddAppToStartUp("BetterTaskBar");
				InIFileSetValue("Settings", "StartUp", "true", std::ios::ate);
			}
			break;
		default: return TRUE;
		}

		break;
	}
	case WM_NOTIFY: 
	{
		LPNMHDR some_item = AutoCast<LPARAM>(lParam);

		if (some_item->idFrom == 10002 || some_item->idFrom == 10003 && some_item->code == -12)
		{
			LPNMCUSTOMDRAW item = AutoCast<LPNMHDR>(some_item);

			FillRect(item->hdc, &item->rc, CreateSolidBrush(RGB(220, 220, 220)));

			if (item->uItemState & CDIS_HOT)
				FillRect(item->hdc, &item->rc, CreateSolidBrush(RGB(210, 210, 210)));	

			if (item->uItemState & CDIS_SELECTED)
				FillRect(item->hdc, &item->rc, CreateSolidBrush(RGB(200, 200, 200)));		
		}

		break;
	}
	case WM_APP:
	{
		switch (lParam)
		{
			case WM_LBUTTONDBLCLK:
			{
				ShowWindow(hwnd, SW_SHOW);
				return 0;
			}
			case WM_RBUTTONUP:
			{
				SetForegroundWindow(hwnd);
				if (InIFileGetValue("Settings", "StartUp") == "true") checked = true;
				ShowPopupMenu(hwnd, checked);
				PostMessage(hwnd, WM_APP + 1, 0, 0);
				return 0;
			}
			default: return TRUE;
		}
	}
	return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

// using 'WinMain' to hide the console 
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	WNDCLASSW ws{ NULL, WndProc, NULL, NULL, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(101)), NULL, CreateSolidBrush(RGB(240, 240, 240)), NULL, L"BetterTaskBarWClass" };
	RegisterClassW(&ws);
	HWND win = CreateWindowExW(WS_EX_LAYERED, ws.lpszClassName, L"TaskBar Congregation", WS_SYSMENU | WS_VISIBLE, 500, 150, 550, 500, NULL, NULL, NULL, NULL);
	SetLayeredWindowAttributes(win, RGB(0, 0, 1), 200, 2);

	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

	BOOL Ret;
	MSG msg;
	
	while ((Ret = GetMessageW(&msg, 0, 0, 0)))
	{
		if (Ret == -1) return -1;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return AutoCast<WPARAM>(msg.wParam).ToAuto<int>();
}
