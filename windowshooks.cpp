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
#include "BtnLifeCicle.h"
#include "BufferHelper.h"

// Global variables
extern HHOOK gKHook;
extern BtnLifeCicle *gLc;
extern BufferHelper *gBuf;

bool hookIsActive = true;

LRESULT CALLBACK WindowsKeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (hookIsActive && nCode >= 0 && nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT   *ks = (KBDLLHOOKSTRUCT*)lParam;

		_RPT5(_CRT_WARN, "key: %d %02X (wp=%d, flags=%d, scan=%d)\n", ks->vkCode, ks->vkCode, wParam, ks->flags, ks->scanCode);

		gLc->begTransition(ks->vkCode, wParam);
		
		gBuf->store(ks->vkCode, wParam);

		if (gLc->isNeedSwitch() || gLc->isNeedTranslate()) {
			HWND hWnd = GetForegroundWindow();
			if (hWnd) {
				if (gLc->isNeedSwitch()) {
					_RPT0(_CRT_WARN, "======================== Switched to next ============================\n");
					hookIsActive = false;
					gBuf->nextLang(hWnd);
					hookIsActive = true;
				} else if (gLc->isNeedTranslate()) {
					_RPT0(_CRT_WARN, "++++++++++++++++++ Translate (ret 1) ++++++++++++++++++++++++++\n");
					hookIsActive = false;
                    gBuf->replay(hWnd);
					hookIsActive = true;
				}

				gLc->endTransition();
				return 1;
			}
		} else if (gLc->isProcessed()) {
			gLc->endTransition();
			return 1;
		}

		gLc->endTransition();
	}

	return CallNextHookEx(gKHook, nCode, wParam, lParam);
}