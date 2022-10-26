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
#include "BufferHelper.h"

#define BUFFER_LENGTH 1024

const TCHAR *SKYPE = "Skype";
const TCHAR *CYR = "®…÷” ≈Õ√ÿŸ«’⁄‘€¬¿œ–ŒÀƒ∆›ﬂ◊—Ã»“‹¡ﬁ∏ÈˆÛÍÂÌ„¯˘Áı˙Ù˚‚‡ÔÓÎ‰Ê˝ˇ˜ÒÏËÚ¸·˛";
const TCHAR *LAT = R"(~QWERTYUIOP{}ASDFGHJKL:"ZXCVBNM<>`qwertyuiop[]asdfghjkl;'zxcvbnm,.)";
const size_t CYR_LAT_LEN = strlen(CYR);

void BufferHelper::store(DWORD vkCode, WPARAM wParam) {
	if (clearAfterReplay && vkCode != translateKey) {
		_RPT1(_CRT_WARN, "<<<=== buffer cleared (after replay): cnt=%d\n", buffer.size());
		clearAfterReplay = false;
		clearBuffer();
	}

	if (isNeedToBuffer(vkCode)) {
		buffer.push_back(BufferedEvent(vkCode, wParam));
	} else if (isNeedToClear(vkCode)) {
		if (!buffer.empty()) {
			_RPT1(_CRT_WARN, "<<<=== buffer cleared: cnt=%d\n", buffer.size());
			clearBuffer();
		}
	}
}

void BufferHelper::nextLang(HWND hWnd) {
	doNextLang(hWnd);
	_RPT1(_CRT_WARN, "<<<=== buffer cleared (after switch): cnt=%d\n", buffer.size());
	clearBuffer();
}

void BufferHelper::doNextLang(HWND hWnd) {
	PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)HKL_NEXT);
}

void BufferHelper::replay(HWND hWnd) {
	TCHAR szBuf[BUFFER_LENGTH];
	GetWindowText(hWnd, szBuf, BUFFER_LENGTH);

	_RPTW1(_CRT_WARN, _T("=== replay (%s) ===\n"), szBuf);

	TCHAR *skype = strstr(szBuf, SKYPE);
	if (skype == NULL) {
		replayDefault(hWnd);
	} else {
		replaySkypeHack(hWnd);
	}
}

void BufferHelper::replayDefault(HWND hWnd) {
	_RPT0(_CRT_WARN, "--- default ---\n");

	if (!buffer.empty()) {
		int keyCnt = 0;
		INPUT *ip = new INPUT[buffer.size() + (cursorPos * 2)];

		for (int i = 0; i < cursorPos; i++) {
			add(ip, &keyCnt, VK_RIGHT);
		}
		for (std::deque<BufferedEvent>::const_iterator i = buffer.begin(); i != buffer.end(); ++i) {
			if ((i->vkCode == VK_BACK || i->vkCode == VK_DELETE) && i->wParam == WM_KEYUP) {
				keyCnt -= 2;
			} else if (isPrinable(i->vkCode) && i->wParam == WM_KEYUP) {
				_RPT1(_CRT_WARN, "%02X->", i->vkCode);
				add(ip, &keyCnt, VK_BACK);
			}
		}

		SendInput(keyCnt, ip, sizeof(INPUT));

		doNextLang(hWnd);

		keyCnt = 0;

		for (std::deque<BufferedEvent>::const_iterator i = buffer.begin(); i != buffer.end(); ++i) {
			addInput(ip, keyCnt++, (WORD)i->vkCode, i->wParam == WM_KEYDOWN ? 0 : KEYEVENTF_KEYUP);
		}

		SendInput(keyCnt, ip, sizeof(INPUT));

		delete ip;

		_RPT0(_CRT_WARN, "===>>> buffer will be cleared ASAP\n");
		clearAfterReplay = true;
	}
}

void BufferHelper::replaySkypeHack(HWND hWnd) {
	int  idActive = GetWindowThreadProcessId(hWnd, NULL);
	if (AttachThreadInput(GetCurrentThreadId(), idActive, TRUE)) {
		HWND  hwndFocused = GetFocus();
		if (hwndFocused != NULL) {
			// Get text from focused window
			TCHAR szBuf[BUFFER_LENGTH];
			SendMessage(hwndFocused, WM_GETTEXT, BUFFER_LENGTH, (LPARAM)szBuf);
			_RPTW1(_CRT_WARN, _T("focused get text: %s\n"), szBuf);

			// Convert text
			size_t len = strlen(szBuf);

			for (size_t i = 0; i < len; i++) {
				TCHAR ch = szBuf[i];
				for (size_t j = 0; j < CYR_LAT_LEN; j++) {
					if (ch == CYR[j]) {
						szBuf[i] = LAT[j];
					} else if (ch == LAT[j]) {
						szBuf[i] = CYR[j];
					}
				}
			}

			_RPTW1(_CRT_WARN, _T("converted: %s\n"), szBuf);

			// Send text back
			SendMessage(hwndFocused, WM_SETTEXT, 0, (LPARAM)szBuf);

			// Move to the end
			int keyCnt = 0;
			INPUT *ip = new INPUT[2];
            add(ip, &keyCnt, VK_END);
			SendInput(keyCnt, ip, sizeof(INPUT));
			delete ip;

			// Switch language
			doNextLang(hWnd);
		}
		AttachThreadInput(GetCurrentThreadId(), idActive, FALSE);
	}

	if (!buffer.empty()) {
		_RPT1(_CRT_WARN, "<<<=== buffer cleared (skype): cnt=%d\n", buffer.size());
		clearBuffer();
	}
}

bool BufferHelper::isNeedToBuffer(DWORD vkCode) {
	if (isPrinable(vkCode)) {
		return true;
	} else if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) {
		return true;
	} else if (!buffer.empty()) {
		if (vkCode == VK_LEFT) {
			cursorPos++;
			return true;
		} else if (vkCode == VK_RIGHT) {
			if (cursorPos > 0) {
				cursorPos--;
			}
			return true;
		} else if (vkCode == VK_BACK) {
			return true;
		} else if (cursorPos>0 && vkCode == VK_DELETE) {
			cursorPos--;
			return true;
		} else if (vkCode == VK_END) {
			cursorPos = 0;
			return true;
		}
	}
	return false;
};

bool BufferHelper::isPrinable(DWORD vkCode) {
	if (vkCode >= 0x30 && vkCode <= 0x39) {
		return true;
	} else if (vkCode >= 0x41 && vkCode <= 0x5A) {
		return true;
	} else if (vkCode == VK_SPACE) {
		return true;
	} else if (vkCode >= VK_OEM_1 && vkCode <= VK_OEM_3) {
		return true;
	} else if (vkCode >= VK_OEM_4 && vkCode <= VK_OEM_8) {
		return true;
	}
	return false;
};

bool BufferHelper::isNeedToClear(DWORD vkCode) {
	if (vkCode == translateKey || vkCode == switchKey) {
		return false;
	} else if (cursorPos < 1 && vkCode == VK_DELETE) {
		return false;
	}
	return true;
};

void BufferHelper::addInput(INPUT *ip, int ind, WORD wVk, DWORD dwFlags) { //TODO ind and ip
	_RPT1(_CRT_WARN, "R->%02X\n", wVk);

	ip[ind].type = INPUT_KEYBOARD;
	ip[ind].ki.wScan = 0; // hardware scan code for key
	ip[ind].ki.time = 0;
	ip[ind].ki.dwExtraInfo = 0;

	// Press the "A" key
	ip[ind].ki.wVk = wVk; // virtual-key code
	ip[ind].ki.dwFlags = dwFlags; // 0 for key press
}

void BufferHelper::add(INPUT *ip, int *ind, WORD wVk) {//TODO ind and ip
	addInput(ip, *ind, wVk, 0);
	*ind = *ind + 1;
	addInput(ip, *ind, wVk, KEYEVENTF_KEYUP);
	*ind = *ind + 1;
}

void BufferHelper::clearBuffer(void) {
	buffer.clear();
	cursorPos = 0;
}

BufferHelper::~BufferHelper() {
	_RPT0(_CRT_WARN, "~BufferHelper\n");
	clearBuffer();
}
