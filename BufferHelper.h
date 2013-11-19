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
	BufferedEvent(DWORD  c, WPARAM p) :vkCode(c), wParam(p) {}

	BufferedEvent(const BufferedEvent& obj) :vkCode(obj.vkCode), wParam(obj.wParam) {}
	
	BufferedEvent& operator=(const BufferedEvent& obj) {
		vkCode = obj.vkCode;
		wParam = obj.wParam;
		return *this;
	}
	
	~BufferedEvent() {}

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

	void replayDefault(HWND hWnd);
	void replaySkypeHack(HWND hWnd);

	bool isNeedToBuffer(DWORD vkCode);
	bool isPrinable(DWORD vkCode);
	bool isNeedToClear(DWORD vkCode);

	void addInput(INPUT *ip, int ind, WORD wVk, DWORD dwFlags);
	void BufferHelper::add(INPUT *ip, int *ind, WORD wVk);
	void doNextLang(HWND hWnd);
	void clearBuffer(void);
};

