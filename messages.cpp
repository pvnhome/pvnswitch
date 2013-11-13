#include "stdafx.h"

void ExitProcessWithMessage(const UINT uExitCode, const TCHAR *msg) {
	MessageBox(NULL, msg, _T("Error"), MB_OK | MB_ICONERROR);
	ExitProcess(uExitCode);
}
