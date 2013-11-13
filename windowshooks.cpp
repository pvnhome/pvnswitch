#include "stdafx.h"
#include "CtlCapsLockLifeCicle.h"
//#include "windowshooks.h"

// Global variables
extern HHOOK gKHook;
extern CtlCapsLockLifeCicle *gLc;

LRESULT CALLBACK WindowsKeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(gKHook, nCode, wParam, lParam);
	}

	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT   *ks = (KBDLLHOOKSTRUCT*)lParam;

		//_RPT5(_CRT_WARN, "key: %d=>VK_CAPITAL=%d (%d=>WM_KEYDOWN=%d,WM_KEYUP=%d)\n", ks->vkCode, VK_CAPITAL, wParam, WM_KEYDOWN, WM_KEYUP);
		_RPT4(_CRT_WARN, "key: %d (wp=%d, flags=%d, scan=%d)\n", ks->vkCode, wParam, ks->flags, ks->scanCode);

		/*
		if (ks->vkCode == VK_CAPITAL) {
		_RPT0(_CRT_WARN, "VK_CAPITAL: return 1)\n");
		return 1;
		}
		*/

		gLc->transition(ks->vkCode, wParam);
		if (gLc->isNeedSwitch()) {
			HWND hWnd = GetForegroundWindow();
			if (hWnd) {
				PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)HKL_NEXT);
				_RPT0(_CRT_WARN, "======================== Switched to next ============================\n");
			}
			//_RPT0(_CRT_WARN, "(ret 1)\n");
			return 1;
		} else if (gLc->isNeedTranslate()) {
			_RPT0(_CRT_WARN, "++++++++++++++++++ Translate (ret 1) ++++++++++++++++++++++++++\n");
			return 1;
		} else if (ks->vkCode == VK_CAPITAL) {
			//_RPT0(_CRT_WARN, "VK_CAPITAL (ret 1)\n");
			return 1;
		}

		/*
		if (ks->vkCode == gSwitchKey) {
		if (wParam == WM_KEYDOWN) {
		HWND hWnd = GetForegroundWindow();
		if (hWnd) {
		PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)HKL_NEXT);
		_RPT1(_CRT_WARN, "Switched to hext = %d\n", ++cnt);

		int ind = 0;
		INPUT ip[12];

		add(ip, &ind, VK_BACK);
		add(ip, &ind, VK_BACK);
		add(ip, &ind, VK_BACK);
		add(ip, &ind, 0x41);
		add(ip, &ind, 0x42);
		add(ip, &ind, 0x43);

		SendInput(12, ip, sizeof(INPUT));
		}
		}
		return 1;
		}
		*/
	}

	return CallNextHookEx(gKHook, nCode, wParam, lParam);
}
