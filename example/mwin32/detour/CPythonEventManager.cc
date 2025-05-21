#include "MEM_guardedalloc.h"

#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "CPythonEventManager.hh"

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

fnCPythonEventManager_GetScriptEventIndex CPythonEventManager_GetScriptEventIndex;
fnCPythonEventManager_ProcessEventSet CPythonEventManager_ProcessEventSet;
fnCPythonEventManager_CheckEventSetIndex CPythonEventManager_CheckEventSetIndex;
fnCPythonEventManager_SelectAnswer CPythonEventManager_SelectAnswer;
fnCPythonEventManager_EndEventProcess CPythonEventManager_EndEventProcess;
fnCPythonEventManager_ClearEventSeti CPythonEventManager_ClearEventSeti;
 
/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

/**
 * This is not an actual function as it is optimized out of the compiler and converted into a static memory location!
 *
 * \details In order to hook this we find a function call to a CPythonEventManager using the CPythonEventManager::Instance() method.
 * The static memory holding the pointer to the static CPythonEventManager instance will be the parameter passed using the ECX register.
 */
CPythonEventManager *CPythonEventManager_Instance() {
	void *lpEventManager = NULL;
	if (!lpEventManager) {
		/**
		 * The pattern locates a sequence of bytes in memory, and the resulting address points
		 * just after the end of this byte sequence. This address corresponds to a static memory
		 * block holding a pointer to a pointer (since it's a reference in C++).
		 * 
		 * How to find;
		 * The function CPythonNetworkStream::RecvScriptPacket() has two strings;
		 * 1. "RecvScriptPacket_RecvError" 
		 * 2. "RecvScriptPacket_SizeError"
		 * 
		 * bool CPythonNetworkStream::RecvScriptPacket() {
		 *	...
		 *	if (...) {
		 *		TraceError("RecvScriptPacket_RecvError");
		 *		return false;
		 *	}
		 *	...
		 *	if (...) {
		 *		TraceError("RecvScriptPacket_SizeError");
		 *		return false;
		 *	}
		 *	...
		 *	int iIndex = CPythonEventManager::Instance().RegisterEventSetFromString(str);
		 *	if (-1 != iIndex)
		 *	{
		 *		CPythonEventManager::Instance().SetVisibleLineCount(iIndex, 30);
		 *		...
		 *	}
		 *	...
		 * }
		 */
		HMODULE hModule = GetModuleHandle(NULL);
		lpEventManager = (void *)LIB_pattern_offset(hModule, DetourGetModuleSize(hModule), "8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b f0 83 fe ff 74 1f 8b 0d");
		ALWAYS_ASSERT(lpEventManager);
	}
	return **(CPythonEventManager ***)POINTER_OFFSET(lpEventManager, 20 /**The number of bytes in the pattern */);
}

void *CPythonEventManager_mPyInterface(CPythonEventManager *me) {
	return *(void **)POINTER_OFFSET(me, 100);
}

/** \} */

void CPythonEventManager_Attach(void *pLocation, size_t dwLength) {
	(void)CPythonEventManager_Instance();
	CPythonEventManager_GetScriptEventIndex = (fnCPythonEventManager_GetScriptEventIndex)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 2c a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b 45 08 8b 55 0c");
	CPythonEventManager_ProcessEventSet = (fnCPythonEventManager_ProcessEventSet)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 6a ff 68 ?? ?? ?? ?? 64 a1 ?? ?? ?? ?? 50 83 ec 60 a1 ?? ?? ?? ?? 33 c5 89 45 f0 53 56 57 50 8d 45 f4 64 a3 ?? ?? ?? ?? 8b f9 89 7d a8");
	CPythonEventManager_CheckEventSetIndex = (fnCPythonEventManager_CheckEventSetIndex)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 8b 55 08 85 d2 78 12 8b 41 10");
	CPythonEventManager_SelectAnswer = (fnCPythonEventManager_SelectAnswer)LIB_pattern_offset(pLocation, dwLength, "55 8b ec ff 75 08 e8 a5 e4 ff ff 84 c0");
	CPythonEventManager_EndEventProcess = (fnCPythonEventManager_EndEventProcess)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 56 57 8b 7d 08 8b f1 57 e8 20 fe ff ff");
	CPythonEventManager_ClearEventSeti = (fnCPythonEventManager_ClearEventSeti)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 56 57 8b 7d 08 8b f1 57 e8 c0 ff ff ff");
	ALWAYS_ASSERT(CPythonEventManager_GetScriptEventIndex);
	ALWAYS_ASSERT(CPythonEventManager_ProcessEventSet);
	ALWAYS_ASSERT(CPythonEventManager_CheckEventSetIndex);
	ALWAYS_ASSERT(CPythonEventManager_SelectAnswer);
	ALWAYS_ASSERT(CPythonEventManager_EndEventProcess);
	ALWAYS_ASSERT(CPythonEventManager_ClearEventSeti);
}

void CPythonEventManager_Detach() {
}
