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

void BtnLifeCicle::begTransition(DWORD vkCode, WPARAM wParam) {
	if (vkCode == switchKey && wParam == WM_KEYUP) {
		needSwitch = true;
	} else if (vkCode == translateKey && wParam == WM_KEYUP) {
		needTranslate = true;
	}
	if (vkCode == translateKey || vkCode == switchKey) {
		processed = true;
	}
}

bool BtnLifeCicle::isNeedSwitch() {
	return needSwitch;
}

bool BtnLifeCicle::isNeedTranslate() {
	return needTranslate;
}

bool BtnLifeCicle::isProcessed() {
	return processed;
}

void BtnLifeCicle::endTransition(void) {
	needSwitch = false;
	needTranslate = false;
	processed = false;
}

BtnLifeCicle::~BtnLifeCicle(void) {
}
