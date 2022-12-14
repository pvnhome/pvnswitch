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

#pragma once

#include <windows.h>

class BtnLifeCicle {
public:
   BtnLifeCicle(DWORD pTranslate = VK_APPS, DWORD pSwitch = VK_CAPITAL) {
      translateKey = pTranslate;
      switchKey = pSwitch;
   }

   void begTransition(DWORD vkCode, WPARAM wParam);
   bool isNeedSwitch();
   bool isNeedTranslate();
   bool isProcessed();
   bool isPrintable();
   void endTransition(void);

   ~BtnLifeCicle(void);

private:
   enum State {
      state_input,
      state_ctrl,
      state_not_active
   };

   State state = state_input;

   DWORD vkCode;
   WPARAM wParam;

   DWORD translateKey;
   DWORD switchKey;

   bool needSwitch = false;
   bool needTranslate = false;
   bool processed = false;
   bool printable = false;

   void processInputState();
   void processCtrlState();

   bool checkPrintable();
};
