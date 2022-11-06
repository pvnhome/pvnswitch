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
#include "BufferHelper.h"

#define BUFFER_LENGTH 2048

const TCHAR *SKYPE = "Skype";
const TCHAR *CYR = "®…÷” ≈Ќ√Ўў«’Џ‘џ¬јѕ–ќЋƒ∆Ёя„—ћ»“№ЅёЄйцукенгшщзхъфывапролджэ€чсмитьбю";
const TCHAR *LAT = R"(~QWERTYUIOP{}ASDFGHJKL:"ZXCVBNM<>`qwertyuiop[]asdfghjkl;'zxcvbnm,.)";
const size_t CYR_LAT_LEN = strlen(CYR);

void BufferHelper::store(DWORD vkCode, WPARAM wParam) {
   needSwitch = false;
   needTranslate = false;
   processed = false;

   if (vkCode == translateKey || vkCode == switchKey) {
      processed = true;

      if (vkCode == switchKey && wParam == WM_KEYUP) {
         needSwitch = true;
      } else if (vkCode == translateKey && wParam == WM_KEYUP) {
         needTranslate = true;
      }
   } else {
      if (clearAfterReplay) {
         LOGF1("<<<=== buffer cleared (after replay): cnt=%lld", buffer.size());
         clearAfterReplay = false;
         clearBuffer();
      }

      if (ctrlState) {
         if (wParam == WM_KEYDOWN && isNeedToBuffer(vkCode, wParam)) {
            LOG("<<<=== ctrl state off");
            ctrlState = false;
            if (vkCode == VK_SPACE && buffer.empty()) {
               LOG("   - first space");
            } else {
               LOGF3("   > %02lX (size=%lld, pos=%d)", vkCode, buffer.size(), cursorPos);
               buffer.push_back(BufferedEvent(vkCode, wParam));
            }
         } else {
            LOG("   - ctrl state");
         }
      } else if (altGrState) {
         if (wParam == WM_KEYUP && vkCode == VK_LCONTROL) {
            LOG("   - system ctrl up");
            altGrState = false;
            processed = true;
         } else {
            LOG("   - system ctrl");
         }
      } else if (wParam == WM_SYSKEYDOWN && vkCode == VK_LCONTROL) {
         LOG("   - system ctrl down");
         altGrState = true;
         processed = true;
      } else if (vkCode == VK_SPACE && buffer.empty()) {
         LOG("   - first space");
      } else if (isNeedToBuffer(vkCode, wParam)) {
         LOGF3("   > %02lX (size=%lld, pos=%d)", vkCode, buffer.size(), cursorPos);
         buffer.push_back(BufferedEvent(vkCode, wParam));
      } else if (isNeedToClear(vkCode, wParam)) {
         LOG(">>>=== ctrl state on");
         if (!buffer.empty()) {
            LOGF1("<<<=== buffer cleared: cnt=%lld", buffer.size());
            clearBuffer();
         } else {
            LOG("   - buffer empty");
         }
         ctrlState = true;
      } else {
         LOGF3("   > %02lX (skip, size=%lld, pos=%d)", vkCode, buffer.size(), cursorPos);
      }
   }
}

void BufferHelper::nextLang(HWND hWnd) {
   doNextLang(hWnd);
   LOGF1("<<<=== buffer cleared (after switch): cnt=%lld", buffer.size());
   clearBuffer();
}

void BufferHelper::doNextLang(HWND hWnd) {
   PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM) HKL_NEXT);
}

void BufferHelper::replay(HWND hWnd) {
   TCHAR szBuf[BUFFER_LENGTH];
   GetWindowText(hWnd, szBuf, BUFFER_LENGTH);

   LOGF1("=== replay (%s) ===", szBuf);

   TCHAR *skype = strstr(szBuf, SKYPE);
   if (skype == NULL) {
      replayDefault(hWnd);
   } else {
      replaySkypeHack(hWnd);
   }
}

void BufferHelper::replayDefault(HWND hWnd) {
   LOG("--- default ---");

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
            LOGF1("%02lX->", i->vkCode);
            add(ip, &keyCnt, VK_BACK);
         }
      }

      SendInput(keyCnt, ip, sizeof(INPUT));

      doNextLang(hWnd);

      keyCnt = 0;

      for (std::deque<BufferedEvent>::const_iterator i = buffer.begin(); i != buffer.end(); ++i) {
         addInput(ip, keyCnt++, (WORD) i->vkCode, i->wParam == WM_KEYDOWN ? 0 : KEYEVENTF_KEYUP);
      }

      SendInput(keyCnt, ip, sizeof(INPUT));

      delete[] ip;

      LOG("===>>> buffer will be cleared ASAP");
      clearAfterReplay = true;
   }
}

void BufferHelper::replaySkypeHack(HWND hWnd) {
   int idActive = GetWindowThreadProcessId(hWnd, NULL);
   if (AttachThreadInput(GetCurrentThreadId(), idActive, TRUE)) {
      HWND hwndFocused = GetFocus();
      if (hwndFocused != NULL) {
         // Get text from focused window
         TCHAR szBuf[BUFFER_LENGTH];
         SendMessage(hwndFocused, WM_GETTEXT, BUFFER_LENGTH, (LPARAM) szBuf);
         LOGF1("focused get text: %s", szBuf);

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

         LOGF1("converted: %s", szBuf);

         // Send text back
         SendMessage(hwndFocused, WM_SETTEXT, 0, (LPARAM) szBuf);

         // Move to the end
         int keyCnt = 0;
         INPUT *ip = new INPUT[2];
         add(ip, &keyCnt, VK_END);
         SendInput(keyCnt, ip, sizeof(INPUT));
         delete[] ip;

         // Switch language
         doNextLang(hWnd);
      }
      AttachThreadInput(GetCurrentThreadId(), idActive, FALSE);
   }

   if (!buffer.empty()) {
      LOGF1("<<<=== buffer cleared (skype): cnt=%lld", buffer.size());
      clearBuffer();
   }
}

bool BufferHelper::isNeedToBuffer(DWORD vkCode, WPARAM wParam) {
   if (isPrinable(vkCode)) {
      return true;
   } else if ((vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) && wParam == WM_KEYDOWN && buffer.empty()) {
      return true;
   } else if (!buffer.empty()) {
      BufferedEvent &lastEvent = buffer.back();

      // Ўифт должен записыватьс€ если после него идут печатные символы.
      // ¬ противном случае буфер должен сбрасыватьс€.
      if (lastEvent.wParam == WM_KEYDOWN && (lastEvent.vkCode == VK_LSHIFT || lastEvent.vkCode == VK_RSHIFT)) {
         if (wParam == WM_KEYDOWN && (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT)) {
            LOG("   - repeated shift");
            return false;
         }
         if (wParam == WM_KEYUP && (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT)) {
            return true;
         }
         if (!(vkCode >= 0x30 && vkCode <= 0x39) && !(vkCode >= 0x41 && vkCode <= 0x5A)) {
            LOG("   - ctrl after shift");
            return false;
         }
      }

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
      } else if (cursorPos > 0 && vkCode == VK_DELETE) {
         cursorPos--;
         return true;
      } else if (vkCode == VK_END) {
         cursorPos = 0;
         return true;
      } else if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) {
         return true;
      }
   }
   return false;
}

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
}

bool BufferHelper::isNeedToClear(DWORD vkCode, WPARAM wParam) {
   if (cursorPos < 1 && vkCode == VK_DELETE) {
      return false;
   } else if (wParam == WM_KEYDOWN && (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT)) {
      return false;
   }
   return true;
}

bool BufferHelper::isNeedSwitch() {
   return needSwitch;
}

bool BufferHelper::isNeedTranslate() {
   return needTranslate;
}

bool BufferHelper::isProcessed() {
   return processed;
}

void BufferHelper::addInput(INPUT *ip, int ind, WORD wVk, DWORD dwFlags) { //TODO ind and ip
   LOGF1("R->%02X", wVk);

   ip[ind].type = INPUT_KEYBOARD;
   ip[ind].ki.wScan = 0; // hardware scan code for key
   ip[ind].ki.time = 0;
   ip[ind].ki.dwExtraInfo = 0;

   // Press the "A" key
   ip[ind].ki.wVk = wVk; // virtual-key code
   ip[ind].ki.dwFlags = dwFlags; // 0 for key press
}

void BufferHelper::add(INPUT *ip, int *ind, WORD wVk) { //TODO ind and ip
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
   LOG("~BufferHelper");
   clearBuffer();
}
