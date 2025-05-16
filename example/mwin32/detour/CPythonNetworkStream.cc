#include "MEM_guardedalloc.h"

#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Chat.h"
#include "intern/Log.h"

#include "CNetStream.hh"
#include "CPythonEventManager.hh"
#include "CPythonNetworkStream.hh"

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

fnCPythonNetworkStream_RecvWhisperPacket CPythonNetworkStream_RecvWhisperPacket = NULL;
fnCPythonNetworkStream_SendWhisperPacket CPythonNetworkStream_SendWhisperPacket = NULL;

fnCPythonNetworkStream_SendScriptAnswerPacket CPythonNetworkStream_SendScriptAnswerPacket = NULL;
fnCPythonNetworkStream_SendScriptButtonPacket CPythonNetworkStream_SendScriptButtonPacket = NULL;
fnCPythonNetworkStream_SendQuestInputStringPacket CPythonNetworkStream_SendQuestInputStringPacket = NULL;
fnCPythonNetworkStream_SendQuestConfirmPacket CPythonNetworkStream_SendQuestConfirmPacket = NULL;

fnCPythonNetworkStream_OnScriptEventStart CPythonNetworkStream_OnScriptEventStart = NULL;

fnCPythonNetworkStream_CheckPacket CPythonNetworkStream_CheckPacket = NULL;

fnCPythonNetworkStream_RecvScriptPacket CPythonNetworkStream_RecvScriptPacket = NULL;
fnCPythonNetworkStream_RecvQuestInfoPacket CPythonNetworkStream_RecvQuestInfoPacket = NULL;
fnCPythonNetworkStream_RecvQuestConfirmPacket CPythonNetworkStream_RecvQuestConfirmPacket = NULL;

fnCPythonNetworkStream_GetAccountCharacterSlotDataz CPythonNetworkStream_GetAccountCharacterSlotDataz = NULL;
fnCPythonNetworkStream_GetAccountCharacterSlotDatau CPythonNetworkStream_GetAccountCharacterSlotDatau = NULL;

fnCPythonNetworkStream___RefreshInventoryWindow CPythonNetworkStream___RefreshInventoryWindow = NULL;

BOOL bExpectScriptPacket = FALSE;

void __fastcall mCPythonNetworkStream___RefreshInventoryWindow(CPythonNetworkStream *me, void *EDX) {
	if (!bExpectScriptPacket && CPythonNetworkStream_SendScriptButtonPacket(me, 2147483700)) {
		bExpectScriptPacket |= TRUE;
	}
	return CPythonNetworkStream___RefreshInventoryWindow(me);
}

WCHEAT_INLINE bool mCPythonNetworkStream_RecvScriptPacketDo(void *me, int iSize, int iAnswer) {
	if (bExpectScriptPacket) {
		char *szPacket = (char *)MEM_mallocN(iSize, "RecvScriptPacket::Override::Packet");
		CNetworkStream_Recv(me, iSize, szPacket);
		bExpectScriptPacket &= (iAnswer != 254);
		MEM_freeN(szPacket);
		return CPythonNetworkStream_SendScriptAnswerPacket(me, iAnswer);
	}
	CPythonNetworkStream *nkNetStream = CPythonNetworkStream_Instance();
	if (nkNetStream) {
		uint32_t uSlot = *CPythonNetworkStream_mSelectedCharacterIndex(nkNetStream);
		uint32_t uLevel = CPythonNetworkStream_GetAccountCharacterSlotDatau(nkNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_LEVEL);
		Logf("%u:%s[Lv %u] Packet::NotHandled", uSlot, CPythonNetworkStream_GetAccountCharacterSlotDataz(nkNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_NAME), uLevel);
	}
	return CPythonNetworkStream_RecvScriptPacket(me);
}

bool __fastcall mCPythonNetworkStream_RecvScriptPacket(void *me, void *EDX) {
	char szBuffer[0x7];
	if (CNetworkStream_Peek(me, 0x6, szBuffer)) {
		unsigned int uHash = *(unsigned int *)szBuffer;
		switch (uHash) {
			case 16853293u:
			case 16796717u:
				return mCPythonNetworkStream_RecvScriptPacketDo(me, *(unsigned short *)POINTER_OFFSET(szBuffer, 0x1), 0);
			case 16845101u:
				return mCPythonNetworkStream_RecvScriptPacketDo(me, *(unsigned short *)POINTER_OFFSET(szBuffer, 0x1), 254);
			case 0x0101db2d:
			case 0x0101da2d:
			case 0x0101d92d:
			case 0x0101d82d:
			case 0x0101d72d:
				return mCPythonNetworkStream_RecvScriptPacketDo(me, *(unsigned short *)POINTER_OFFSET(szBuffer, 0x1), 254);
		}
		if (bExpectScriptPacket) {
			Logf("RecvScriptPacket(%xu: [Size: %d Skin: %d])", uHash, (int)*(unsigned short *)POINTER_OFFSET(szBuffer, 0x1), *(unsigned short *)POINTER_OFFSET(szBuffer, 0x3));
			bExpectScriptPacket &= FALSE;
		}
	}

	return CPythonNetworkStream_RecvScriptPacket(me);
}

bool __fastcall mCPythonNetworkStream_RecvWhisperPacket(void *me, void *EDX) {
	char szBuffer[0x200];
	if (!CNetworkStream_Peek(me, 0x1d, szBuffer)) {
		return CPythonNetworkStream_RecvWhisperPacket(me);
	}
	if (!CNetworkStream_Peek(me, *(unsigned short *)POINTER_OFFSET(szBuffer, 0x1), szBuffer)) {
		return CPythonNetworkStream_RecvWhisperPacket(me);
	}
	szBuffer[*(unsigned short *)POINTER_OFFSET(szBuffer, 0x1)] = '\0';

	Chat_PostMessage(me, (const char *)POINTER_OFFSET(szBuffer, 0x4), (const char *)POINTER_OFFSET(szBuffer, 0x1d));
	return CPythonNetworkStream_RecvWhisperPacket(me);
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

/**
 * This is not an actual function as it is optimized out of the compiler and converted into a static memory location!
 * 
 * \details In order to hook this we find a function call to a CPythonNetworkStream using the CPythonNetworkStream::Instance() method.
 * The static memory holding the pointer to the static CPythonNetworkStream instance will be the parameter passed using the ECX register.
 */
CPythonNetworkStream *CPythonNetworkStream_Instance() {
	void *lpNetStream = NULL;
	if (!lpNetStream) {
		/**
		 * The pattern locates a sequence of bytes in memory, and the resulting address points 
		 * just after the end of this byte sequence. This address corresponds to a static memory 
		 * block holding a pointer to a pointer (since it's a reference in C++).
		 */
		HMODULE hModule = GetModuleHandle(NULL);
		lpNetStream = (void *)LIB_pattern_offset(hModule, DetourGetModuleSize(hModule), "57 50 e8 07 eb 08 00 8b 3d");
		WCHEAT_assert(lpNetStream);
	}
	return **(CPythonNetworkStream ***)POINTER_OFFSET(lpNetStream, 9 /**The number of bytes in the pattern */);
}

uint32_t *CPythonNetworkStream_mSelectedCharacterIndex(CPythonNetworkStream *me) {
	return (uint32_t *)POINTER_OFFSET(me, 0x79a0);
}

/** \} */

void CPythonNetworkStream_Attach(void *pLocation, size_t dwLength) {
	(void)CPythonNetworkStream_Instance();
	CPythonNetworkStream_CheckPacket = (fnCPythonNetworkStream_CheckPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 6a ff 68 ?? ?? ?? ?? 64 a1 ?? ?? ?? ?? 50 83 ec 0c 56 57 a1 ?? ?? ?? ?? 33 c5 50 8d 45 f4 64 a3 ?? ?? ?? ?? 8b f1 64 a1 2c 00 00 00");
	CPythonNetworkStream_GetAccountCharacterSlotDataz = (fnCPythonNetworkStream_GetAccountCharacterSlotDataz)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 8b 55 08 83 fa 05 72 09 a1 ?? ?? ?? ?? 5d c2 08 00");
	CPythonNetworkStream_GetAccountCharacterSlotDatau = (fnCPythonNetworkStream_GetAccountCharacterSlotDatau)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 8b 55 08 56 83 fa 05");
	CPythonNetworkStream_SendWhisperPacket = (fnCPythonNetworkStream_SendWhisperPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 24 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b 45 08 56 8b 75 0c");
	CPythonNetworkStream_SendScriptAnswerPacket = (fnCPythonNetworkStream_SendScriptAnswerPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 51 8a 45 08 56 88 45 fd 8b f1 8d 45 fc c6 45 fc 1d");
	CPythonNetworkStream_SendScriptButtonPacket = (fnCPythonNetworkStream_SendScriptButtonPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 08 8b 45 08 56 89 45 f9 8b f1 8d 45 f8 c6 45 f8 42");
	CPythonNetworkStream_SendQuestInputStringPacket = (fnCPythonNetworkStream_SendQuestInputStringPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 48 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b 45 08 56 6a 40");
	CPythonNetworkStream_SendQuestConfirmPacket = (fnCPythonNetworkStream_SendQuestConfirmPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 08 53 8a 5d 08");
	CPythonNetworkStream_RecvScriptPacket = (fnCPythonNetworkStream_RecvScriptPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 08 8d 45 f8 56 50 6a 06 8b f1 e8 4c c1 05 00");
	CPythonNetworkStream_RecvQuestInfoPacket = (fnCPythonNetworkStream_RecvQuestInfoPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 6a ff 68 ?? ?? ?? ?? 64 a1 ?? ?? ?? ?? 50 81 ec 50 01 00 00 a1 ?? ?? ?? ?? 33 c5 89 45 f0 56 57 50 8d 45 f4 64 a3 ?? ?? ?? ?? 8b f9");
	CPythonNetworkStream_RecvQuestConfirmPacket = (fnCPythonNetworkStream_RecvQuestConfirmPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 50 a1 ?? ?? ?? ?? 33 c5 89 45 fc 56");
	CPythonNetworkStream_RecvWhisperPacket = (fnCPythonNetworkStream_RecvWhisperPacket)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 6a ff 68 ?? ?? ?? ?? 64 a1 ?? ?? ?? ?? 50 81 ec 64 02 00 00 a1 ?? ?? ?? ?? 33 c5 89 45 f0 56 57 50 8d 45 f4 64 a3 ?? ?? ?? ?? 8b f9");
	CPythonNetworkStream___RefreshInventoryWindow = (fnCPythonNetworkStream___RefreshInventoryWindow)LIB_pattern_offset(pLocation, dwLength, "c6 81 a3 03 00 00 01 c3");
	WCHEAT_assert(CPythonNetworkStream_CheckPacket);
	WCHEAT_assert(CPythonNetworkStream_GetAccountCharacterSlotDataz);
	WCHEAT_assert(CPythonNetworkStream_GetAccountCharacterSlotDatau);
	WCHEAT_assert(CPythonNetworkStream_SendWhisperPacket);
	WCHEAT_assert(CPythonNetworkStream_SendScriptAnswerPacket);
	WCHEAT_assert(CPythonNetworkStream_SendScriptButtonPacket);
	WCHEAT_assert(CPythonNetworkStream_SendQuestInputStringPacket);
	WCHEAT_assert(CPythonNetworkStream_SendQuestConfirmPacket);
	WCHEAT_assert(CPythonNetworkStream_RecvScriptPacket);
	WCHEAT_assert(CPythonNetworkStream_RecvQuestInfoPacket);
	WCHEAT_assert(CPythonNetworkStream_RecvQuestConfirmPacket);
	WCHEAT_assert(CPythonNetworkStream_RecvWhisperPacket);
	WCHEAT_assert(CPythonNetworkStream___RefreshInventoryWindow);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID *)&CPythonNetworkStream_RecvScriptPacket, mCPythonNetworkStream_RecvScriptPacket);
	DetourAttach((PVOID *)&CPythonNetworkStream_RecvWhisperPacket, mCPythonNetworkStream_RecvWhisperPacket);
	DetourAttach((PVOID *)&CPythonNetworkStream___RefreshInventoryWindow, mCPythonNetworkStream___RefreshInventoryWindow);
	DetourTransactionCommit();
}

void CPythonNetworkStream_Detach() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach((PVOID *)&CPythonNetworkStream_RecvScriptPacket, mCPythonNetworkStream_RecvScriptPacket);
	DetourDetach((PVOID *)&CPythonNetworkStream_RecvWhisperPacket, mCPythonNetworkStream_RecvWhisperPacket);
	DetourDetach((PVOID *)&CPythonNetworkStream___RefreshInventoryWindow, mCPythonNetworkStream___RefreshInventoryWindow);
	DetourTransactionCommit();
}
