#pragma once

#include <windows.h>

class CtlCapsLockLifeCicle
{
public:
   CtlCapsLockLifeCicle(void);

   void transition(DWORD vkCode, WPARAM wParam);
   bool isNeedSwitch();
   bool isNeedTranslate();

   ~CtlCapsLockLifeCicle(void);

private:
   enum State {
      INITIAL, // CapsLock and Control buttons arn't pressed
      L1C0,    // CapsLock button was pressed in INITIAL state
      L1C1,    // Control button was pressed after CapsLock or CapsLock was pressed after Control
      L_C1,    // CapsLock button was released but Control button still pressed
      L1C_,
      C1L0     // Control button was pressed in INITIAL state
   };

   State state;
   DWORD ctrlKey;
   DWORD switchKey;

   bool needSwitch;
   bool needTranslate;

};
