#include "stdafx.h"
#include "CtlCapsLockLifeCicle.h"

CtlCapsLockLifeCicle::CtlCapsLockLifeCicle(void) {
	state = INITIAL;
	ctrlKey = VK_LCONTROL;
	switchKey = VK_CAPITAL;

	needSwitch = FALSE;
	needTranslate = FALSE;
}

void CtlCapsLockLifeCicle::transition(DWORD vkCode, WPARAM wParam) {
	if (vkCode == ctrlKey || vkCode == switchKey) {
		//_RPT3(_CRT_WARN, "transition: %d (%d), state=%d\n", vkCode, wParam, state);

		switch (state) {
		case INITIAL:
			if (vkCode == switchKey) {
				if (wParam == WM_KEYDOWN) {
					_RPT0(_CRT_WARN, "Switched to L1C0\n");
					state = L1C0;
				}
			} else if (vkCode == ctrlKey) {
				if (wParam == WM_KEYDOWN) {
					_RPT0(_CRT_WARN, "Switched to C1L0\n");
					state = C1L0;
				}
			}
			break;

		case L1C0:
			if (vkCode == switchKey) {
				if (wParam == WM_KEYUP) {
					_RPT0(_CRT_WARN, "Switched to INITIAL (needSwitch = TRUE)\n");
					needSwitch = TRUE;
					state = INITIAL;
				}
			} else if (vkCode == ctrlKey) {
				if (wParam == WM_KEYDOWN) {
					_RPT0(_CRT_WARN, "Switched to L1C1\n");
					state = L1C1;
				}
			}
			break;

		case L1C1:
			if (vkCode == switchKey) {
				if (wParam == WM_KEYUP) {
					_RPT0(_CRT_WARN, "Switched to L_C1\n");
					state = L_C1;
				}
			} else if (vkCode == ctrlKey) {
				if (wParam == WM_KEYUP) {
					_RPT0(_CRT_WARN, "Switched to L1C_\n");
					state = L1C_;
				}
			}
			break;

		case L_C1:
			if (vkCode == switchKey) {
				if (wParam == WM_KEYDOWN) {
					_RPT0(_CRT_WARN, "Switched to L1C1\n");
					state = L1C1;
				}
			} else if (vkCode == ctrlKey) {
				if (wParam == WM_KEYUP) {
					_RPT0(_CRT_WARN, "Switched to INITIAL (needTranslate = TRUE)\n");
					needTranslate = TRUE;
					state = INITIAL;
				}
			}
			break;

		case L1C_:
			if (vkCode == switchKey) {
				if (wParam == WM_KEYUP) {
					_RPT0(_CRT_WARN, "Switched to INITIAL (needTranslate = TRUE)\n");
					needTranslate = TRUE;
					state = INITIAL;
				}
			} else if (vkCode == ctrlKey) {
				if (wParam == WM_KEYDOWN) {
					_RPT0(_CRT_WARN, "Switched to L1C1\n");
					state = L1C1;
				}
			}
			break;

		case C1L0:
			if (vkCode == switchKey) {
				if (wParam == WM_KEYDOWN) {
					_RPT0(_CRT_WARN, "Switched to L1C1\n");
					state = L1C1;
				}
			} else if (vkCode == ctrlKey) {
				if (wParam == WM_KEYUP) {
					_RPT0(_CRT_WARN, "Switched to INITIAL\n");
					state = INITIAL;
				}
			}
			break;
		}
	}
}

bool CtlCapsLockLifeCicle::isNeedSwitch() {
	//_RPT0(_CRT_WARN, "isNeedSwitch\n");
	if (needSwitch) {
		//_RPT0(_CRT_WARN, "isNeedSwitch is OK\n");
		needSwitch = FALSE;
		return TRUE;
	}
	return FALSE;
}

bool CtlCapsLockLifeCicle::isNeedTranslate() {
	if (needTranslate) {
		needTranslate = FALSE;
		return TRUE;
	}
	return FALSE;
}


CtlCapsLockLifeCicle::~CtlCapsLockLifeCicle(void) {
}
