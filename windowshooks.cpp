/*
* This file is part of the PvnSwitch (https://github.com/pvnhome/pvnswitch).
*
* Copyright (C) 2013-2022 Victor Pyankov <pvnhome@yandex.ru>
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
//extern BtnLifeCicle *gLc;
extern BufferHelper *gBuf;

bool hookIsActive = true;

LRESULT CALLBACK WindowsKeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (hookIsActive && nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT   *ks = (KBDLLHOOKSTRUCT*)lParam;

#ifndef NDEBUG
		const char* type;
		if (wParam == WM_KEYDOWN) {
			type = "WM_KEYDOWN";
		} else if (wParam == WM_KEYUP) {
			type = "WM_KEYUP";
		} else if (wParam == WM_SYSKEYDOWN) {
			type = "WM_SYSKEYDOWN";
		} else if (wParam == WM_SYSKEYUP) {
			type = "WM_SYSKEYUP";
		} else {
			type = "N/A";
		}
		LOGF5("%s: %ld %02lX (flags=%ld, scan=%ld)", type, ks->vkCode, ks->vkCode, ks->flags, ks->scanCode);

#ifdef DEBUG_LLKHF_FLAGS
		if (ks->flags & LLKHF_EXTENDED) {
		   LOG("   + EXTENDED");
		}
      if (ks->flags & LLKHF_LOWER_IL_INJECTED) {
         LOG("   + IL_INJECTED");
      }
      if (ks->flags & LLKHF_INJECTED) {
         LOG("   + INJECTED");
      }
      if (ks->flags & LLKHF_ALTDOWN) {
         LOG("   + ALTDOWN");
      }
      if (ks->flags & LLKHF_UP) {
         LOG("   + UP");
      }
#endif
#endif

		//gLc->begTransition(ks->vkCode, wParam);

		gBuf->store(ks->vkCode, wParam);

		if (gBuf->isNeedSwitch() || gBuf->isNeedTranslate()) {
			HWND hWnd = GetForegroundWindow();
			if (hWnd) {
				if (gBuf->isNeedSwitch()) {
					LOG("======================== Switched to next ============================");
					hookIsActive = false;
					gBuf->nextLang(hWnd);
					hookIsActive = true;
				} else if (gBuf->isNeedTranslate()) {
					LOG("++++++++++++++++++ Translate (ret 1) ++++++++++++++++++++++++++");
					hookIsActive = false;
               gBuf->replay(hWnd);
					hookIsActive = true;
				}

				//gLc->endTransition();
            LOG("return 1");
				return 1;
			}
		} else if (gBuf->isProcessed()) {
			//gLc->endTransition();
         LOG("return 1");
			return 1;
		}

      //gLc->endTransition();
	}

	return CallNextHookEx(gKHook, nCode, wParam, lParam);
}
