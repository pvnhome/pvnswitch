/*
 * This file is part of the PvnSwitch (https://sourceforge.net/projects/pvnswitch).
 *
 * Copyright (C) 2013 Victor Pyankov <pvnbips@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "stdafx.h"
#include <shellapi.h>
#include "windowshooks.h"
#include "BtnLifeCicle.h"
#include "BufferHelper.h"
#include "messages.h"
#include "pvnswitch.h"

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
HHOOK gKHook;
BufferHelper *gBuf;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
   MSG msg;

   LOG("started");

   //key: 165 A5 (wp=260, flags=33, scan=56)
   //key: 165 A5 (wp=257, flags=129, scan=56)
   //key: 165 A5 (wp=260, flags=33, scan=56)
   //key: 165 A5 (wp=257, flags=129, scan=56)

   // Parse command line params
   DWORD vkCode = VK_RMENU;
   LPWSTR *szArglist;
   int nArgs;
   int i;

   szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
   if (NULL == szArglist) {
      ExitProcessWithMessage(2, "CommandLineToArgvW failed");
   } else {
      LOG("Params:");
      for (i = 0; i < nArgs; i++) {
         LOGF2("%d: %ls", i, szArglist[i]);

         if (wcscmp(szArglist[i], L"-tAPPS") == 0) {
            LOG("Key for correction is VK_APPS");
            vkCode = VK_APPS;
         } else if (wcscmp(szArglist[i], L"-tLCONTROL") == 0) {
            LOG("Key for correction is VK_LCONTROL");
            vkCode = VK_LCONTROL;
         } else if (wcscmp(szArglist[i], L"-tRCONTROL") == 0) {
            LOG("Key for correction is VK_RCONTROL");
            vkCode = VK_RCONTROL;
         } else if (wcscmp(szArglist[i], L"-tLWIN") == 0) {
            LOG("Key for correction is VK_LWIN");
            vkCode = VK_LWIN;
         } else if (wcscmp(szArglist[i], L"-tRWIN") == 0) {
            LOG("Key for correction is VK_RWIN");
            vkCode = VK_RWIN;
         } else if (wcscmp(szArglist[i], L"-tLMENU") == 0) {
            LOG("Key for correction is VK_LMENU");
            vkCode = VK_LMENU;
         } else if (wcscmp(szArglist[i], L"-tRMENU") == 0) {
            LOG("Key for correction is VK_RMENU");
            vkCode = VK_RMENU;
         }
      }
   }

   // Free memory allocated for CommandLineToArgvW arguments.
   LocalFree(szArglist);

   // Initialize global strings
   LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadString(hInstance, IDC_WNDCLASS, szWindowClass, MAX_LOADSTRING);
   MyRegisterClass(hInstance);

   // Perform application initialization:
   if (!InitInstance(hInstance, nCmdShow)) {
      return FALSE;
   }

   gBuf = new BufferHelper(vkCode, VK_FOR_SWITCH);

   // Create keyboard hook:
   gKHook = SetWindowsHookEx(WH_KEYBOARD_LL, WindowsKeyboardHook, GetModuleHandle(NULL), 0);
   if (gKHook == NULL) {
      LOG("gKHook == NULL");
      ExitProcessWithMessage(2, "Error on SetWindowsHookEx()");
   }
   LOG("hooked");

   // Main message loop:
   BOOL bRet;
   while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
      if (bRet == -1) {
         // handle the error and possibly exit
         LOG("exit from loop with bRet == -1");
         break;
      } else {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   // Free resources
   UnhookWindowsHookEx(gKHook);
   LOG("unhooked");

   delete gBuf;
   LOG("LC deleted");
   LOG("stoped");

   return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SYSTRAYMENU);
   wcex.lpszClassName = szWindowClass;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPICON_SMALL));

   return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   HWND hWnd;
   HICON hMainIcon;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd) {
      return FALSE;
   }

   hMainIcon = LoadIcon(hInstance, (LPCTSTR) MAKEINTRESOURCE(IDI_APPICON_SMALL));

   nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes
   nidApp.hWnd = (HWND) hWnd;               //handle of the window which will process this app. messages
   nidApp.uID = IDI_APPICON_SMALL;         //ID of the icon that willl appear in the system tray
   nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags
   nidApp.hIcon = hMainIcon;               // handle of the Icon to be displayed, obtained from LoadIcon
   nidApp.uCallbackMessage = WM_USER_SHELLICON;
   LoadString(hInstance, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);
   Shell_NotifyIcon(NIM_ADD, &nidApp);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   int wmId;
   //int wmId, wmEvent;
   POINT lpClickPoint;
   //HDC hdc;

   switch (message) {
      case WM_USER_SHELLICON:
         // systray msg callback
         switch (LOWORD(lParam)) {
            case WM_RBUTTONDOWN:
               //UINT uFlag = MF_BYPOSITION | MF_STRING;
               GetCursorPos(&lpClickPoint);
               hPopMenu = CreatePopupMenu();
               InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_ABOUT, _T("About"));
               InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, 0, 0);
               InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

               SetForegroundWindow(hWnd);
               TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
               return TRUE;
         }
         break;

      case WM_COMMAND:
         wmId = LOWORD(wParam);
         //wmEvent = HIWORD(wParam);
         // Parse the menu selections:
         switch (wmId) {
            case IDM_ABOUT:
               DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
               break;
            case IDM_EXIT:
               LOG("Exit command")
               ;
               Shell_NotifyIcon(NIM_DELETE, &nidApp);
               DestroyWindow(hWnd);
               break;
            default:
               return DefWindowProc(hWnd, message, wParam, lParam);
         }
         break;
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
   UNREFERENCED_PARAMETER(lParam);
   switch (message) {
      case WM_INITDIALOG:
         return (INT_PTR) TRUE;

      case WM_COMMAND:
         if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR) TRUE;
         }
         break;
   }
   return (INT_PTR) FALSE;
}
