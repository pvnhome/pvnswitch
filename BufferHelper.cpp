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

void BufferHelper::store(DWORD vkCode, WPARAM wParam) {
	if (clearAfterReplay && vkCode != translateKey) {
		_RPT1(_CRT_WARN, "<<<=== buffer cleared (after replay): cnt=%d\n", buffer.size());
		clearAfterReplay = false;
		buffer.clear();
	}

	if (isNeedToBuffer(vkCode)) {
		//BufferedEvent evt(vkCode, wParam); 
		//buffer.push_back(std::move(evt));
		buffer.push_back(BufferedEvent(vkCode, wParam));
		//buffer.push_back(std::move(BufferedEvent(vkCode, wParam)));
	} else if (isNeedToClear(vkCode)) {
		if (!buffer.empty()) {
			_RPT1(_CRT_WARN, "<<<=== buffer cleared: cnt=%d\n", buffer.size());
			buffer.clear();
		}
	}
}

void BufferHelper::nextLang(HWND hWnd) {
	doNextLang(hWnd);
	_RPT1(_CRT_WARN, "<<<=== buffer cleared (after switch): cnt=%d\n", buffer.size());
	buffer.clear();
}

void BufferHelper::doNextLang(HWND hWnd) {
	PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)HKL_NEXT);
}

void BufferHelper::replay(HWND hWnd) {
	if (!buffer.empty()) {
		doNextLang(hWnd);

		int keyCnt = 0;
		INPUT *ip = new INPUT[buffer.size() * 2 + 2];

		_RPT0(_CRT_WARN, "=== replay ===\n");

		add(ip, &keyCnt, VK_END);
		for (std::deque<BufferedEvent>::const_iterator i = buffer.begin(); i != buffer.end(); ++i) {
			if ((i->vkCode == VK_BACK || i->vkCode == VK_DELETE) && i->wParam == WM_KEYUP) {
				keyCnt -= 2;
			} else if (isPrinable(i->vkCode) && i->wParam == WM_KEYUP) {
				_RPT1(_CRT_WARN, "%02X->", i->vkCode);
				add(ip, &keyCnt, VK_BACK);
			}
		}
		
		for (std::deque<BufferedEvent>::const_iterator i = buffer.begin(); i != buffer.end(); ++i) {
			addInput(ip, keyCnt++, (WORD) i->vkCode, i->wParam == WM_KEYDOWN ? 0 : KEYEVENTF_KEYUP);
		}

		SendInput(keyCnt, ip, sizeof(INPUT));

		delete ip;

		_RPT0(_CRT_WARN, "===>>> buffer will be cleared ASAP\n");
		clearAfterReplay = true;
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
	} else if (vkCode == VK_TAB) {
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

void BufferHelper::addInput(INPUT *ip, int ind, WORD wVk, DWORD dwFlags) {
	_RPT1(_CRT_WARN, "R->%02X\n", wVk);

	ip[ind].type = INPUT_KEYBOARD;
	ip[ind].ki.wScan = 0; // hardware scan code for key
	ip[ind].ki.time = 0;
	ip[ind].ki.dwExtraInfo = 0;

	// Press the "A" key
	ip[ind].ki.wVk = wVk; // virtual-key code
	ip[ind].ki.dwFlags = dwFlags; // 0 for key press
}

void BufferHelper::add(INPUT *ip, int *ind, WORD wVk) {
	addInput(ip, *ind, wVk, 0);
	*ind = *ind + 1;
	addInput(ip, *ind, wVk, KEYEVENTF_KEYUP);
	*ind = *ind + 1;
}

BufferHelper::~BufferHelper() {
	_RPT0(_CRT_WARN, "~BufferHelper\n");
	buffer.clear();
}
