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

#pragma once

#include <windows.h>
#include <deque>

class BufferedEvent {
public:
	BufferedEvent(DWORD  c, WPARAM p) :vkCode(c), wParam(p) {
		//_RPT2(_CRT_WARN, "BufferedEvent = %d,%d\n", vkCode, wParam);
	}

	BufferedEvent(const BufferedEvent& obj) :vkCode(obj.vkCode), wParam(obj.wParam) {
		//_RPT2(_CRT_WARN, "BufferedEvent (copy) = %d,%d\n", vkCode, wParam);
	}
	
	BufferedEvent& operator=(const BufferedEvent& obj) {
		vkCode = obj.vkCode;
		wParam = obj.wParam;
		return *this;
	}
	
	// Move constructor (C++11)
	/*
	BufferedEvent(BufferedEvent&& other) {
		_RPT2(_CRT_WARN, "BufferedEvent (move) from %d,%d\n", other.vkCode, other.wParam);
		//vkCode = other.vkCode;
		//wParam = other.wParam;
		//other.vkCode = 0;
		//other.wParam = 0;
		*this = std::move(other);
	}

	BufferedEvent& operator=(BufferedEvent&& other) {
		if (this != &other) {
			vkCode = other.vkCode;
			wParam = other.wParam;
			other.vkCode = 0;
			other.wParam = 0;
		}
		return *this;
	}
	*/

	~BufferedEvent() {
		//_RPT2(_CRT_WARN, "destructor = %d,%d\n", vkCode, wParam);
	}

	DWORD  vkCode;
	WPARAM wParam;
};

class BufferHelper {
public:
	BufferHelper(DWORD pTranslate = VK_APPS, DWORD pSwitch = VK_CAPITAL) {
		translateKey = pTranslate;
		switchKey = pSwitch;
	}

	void store(DWORD vkCode, WPARAM wParam);
	void nextLang(HWND hWnd);
	void replay(HWND hWnd);

	~BufferHelper();

private:
	DWORD translateKey;
	DWORD switchKey;

	int cursorPos = 0;

	bool clearAfterReplay = false;

	std::deque<BufferedEvent> buffer;

	bool isNeedToBuffer(DWORD vkCode);
	bool isPrinable(DWORD vkCode);
	bool isNeedToClear(DWORD vkCode);

	void addInput(INPUT *ip, int ind, WORD wVk, DWORD dwFlags);
	void BufferHelper::add(INPUT *ip, int *ind, WORD wVk);
	void doNextLang(HWND hWnd);
};

