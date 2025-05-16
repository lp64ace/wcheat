#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Chat.h"
#include "intern/Log.h"

#include "CPythonApplication.hh"

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

fnCPythonApplication_UpdateGame CPythonApplication_UpdateGame = NULL;

void __fastcall mCPythonApplication_UpdateGame(void *me, void *EDX) {
	Chat_PollMessage(CPythonApplication_mChat((CPythonApplication *)me));
	CPythonApplication_UpdateGame(me);
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

void *CPythonApplication_mNetworkStream(CPythonApplication *me) {
	return POINTER_OFFSET(me, 0x8f0);
}
void *CPythonApplication_mChat(CPythonApplication *me) {
	return POINTER_OFFSET(me, 0x25a98);
}

/** \} */

void CPythonApplication_Attach(void *pLocation, size_t dwLength) {
	CPythonApplication_UpdateGame = (fnCPythonApplication_UpdateGame)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 6a ff 68 ?? ?? ?? ?? 64 a1 ?? ?? ?? ?? 50 83 ec 4c a1 ?? ?? ?? ?? 33 c5 89 45 f0 53 56 57 50 8d 45 f4 64 a3 ?? ?? ?? ?? 8b f1 89 75 e0");
	WCHEAT_assert(CPythonApplication_UpdateGame);
	
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID *)&CPythonApplication_UpdateGame, mCPythonApplication_UpdateGame);
	DetourTransactionCommit();
}

void CPythonApplication_Detach() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach((PVOID *)&CPythonApplication_UpdateGame, mCPythonApplication_UpdateGame);
	DetourTransactionCommit();
}
