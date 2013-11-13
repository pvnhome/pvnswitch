// PVN Keyboard switcher
//

#include "stdafx.h"
#include "windowshooks.h"
#include "CtlCapsLockLifeCicle.h"
#include "messages.h"

// Global variables
HHOOK gKHook;
CtlCapsLockLifeCicle *gLc;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	MSG msg;

	_RPT0(_CRT_WARN, "started\n");

	HANDLE quit_event = CreateEvent(NULL, false, false, _T("pvnswitch.exe"));
	if (quit_event == NULL) {
		ExitProcessWithMessage(1, _T("Error on CreateEvent()"));
	} else {
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_RPT0(_CRT_WARN, "ERROR_ALREADY_EXISTS\n");
			SetEvent(quit_event);
			//return 0;
			ExitProcessWithMessage(0, _T("Another copy of pwnswitch is running and it will be stoped"));
		}
	}

	_RPT0(_CRT_WARN, "LC created\n");
	gLc = new CtlCapsLockLifeCicle();

	//gKHook = SetWindowsHookEx(WH_KEYBOARD_LL, WindowsKeyboardHook, GetModuleHandle(0), 0);
	//gKHook = SetWindowsHookEx(WH_KEYBOARD_LL, WindowsKeyboardHook, NULL, NULL);
	/*
	*/
	gKHook = SetWindowsHookEx(WH_KEYBOARD_LL, WindowsKeyboardHook, GetModuleHandle(NULL), NULL);
	if (gKHook == NULL) {
		_RPT0(_CRT_WARN, "gKHook == NULL\n");
		ExitProcessWithMessage(2, _T("Error on SetWindowsHookEx()"));
	}
	_RPT0(_CRT_WARN, "hooked\n");

	while (TRUE) {
		//_RPT0(_CRT_WARN, "loop\n");
		int code = MsgWaitForMultipleObjectsEx(1, &quit_event, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
		if (code == WAIT_OBJECT_0) {
			_RPT0(_CRT_WARN, "termination event\n");
			PostQuitMessage(0);
			//continue;
		} else {
			//_RPT1(_CRT_WARN, "wait code = %d\n", code);
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			_RPT1(_CRT_WARN, "message in queue: %d\n", msg.message);

			//_RPT0(_CRT_WARN, "before GetMessage\n");
			BOOL bRet = GetMessage(&msg, NULL, 0, 0);
			if (bRet > 0) {
				_RPT1(_CRT_WARN, "message: %d\n", msg.message);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else if (bRet == 0) {
				_RPT0(_CRT_WARN, "exit from loop\n");
				break;
			} else {
				DWORD dw = GetLastError();
				_RPT1(_CRT_WARN, "ERROR: exit from loop (bRet < 0, error code = %d)\n", dw);
				break;
			}
		} else {
			//_RPT0(_CRT_WARN, "queue empty\n");
		}
	}

	UnhookWindowsHookEx(gKHook);
	_RPT0(_CRT_WARN, "unhooked\n");

	delete gLc;
	_RPT0(_CRT_WARN, "LC deleted\n");

	CloseHandle(quit_event);

	_RPT0(_CRT_WARN, "stoped\n");

	return 0;
}