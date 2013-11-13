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
#include "CtlCapsLockLifeCicle.h"

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
	}

	return CallNextHookEx(gKHook, nCode, wParam, lParam);
}
