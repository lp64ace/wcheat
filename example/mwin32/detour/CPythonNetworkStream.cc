#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "CPythonNetworkStream.hh"

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

fnCPythonNetworkStream_RecvWhisperPacket CPythonNetworkStream_RecvWhisperPacket = NULL;
fnCPythonNetworkStream_SendWhisperPacket CPythonNetworkStream_SendWhisperPacket = NULL;
fnCPythonNetworkStream_GetAccountCharacterSlotDataz CPythonNetworkStream_GetAccountCharacterSlotDataz = NULL;
fnCPythonNetworkStream_GetAccountCharacterSlotDatau CPythonNetworkStream_GetAccountCharacterSlotDatau = NULL;

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
		lpNetStream = NewDetour("57 50 e8 07 eb 08 00 8b 3d");
		WCHEAT_assert(lpNetStream);
	}
	return **(CPythonNetworkStream ***)POINTER_OFFSET(lpNetStream, 9 /**The number of bytes in the pattern */);
}

uint32_t *CPythonNetworkStream_mSelectedCharacterIndex(CPythonNetworkStream *me) {
	return (uint32_t *)POINTER_OFFSET(me, 0x79a0);
}

/** \} */

void CPythonNetworkStream_Detour() {
	(void)CPythonNetworkStream_Instance();
	CPythonNetworkStream_RecvWhisperPacket = (fnCPythonNetworkStream_RecvWhisperPacket)NewDetour("55 8b ec 6a ff 68 ?? ?? ?? ?? 64 a1 ?? ?? ?? ?? 50 81 ec 64 02 00 00 a1 ?? ?? ?? ?? 33 c5 89 45 f0 56 57 50 8d 45 f4 64 a3 ?? ?? ?? ?? 8b f9");
	CPythonNetworkStream_SendWhisperPacket = (fnCPythonNetworkStream_SendWhisperPacket)NewDetour("55 8b ec 83 ec 24 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b 45 08 56 8b 75 0c");
	CPythonNetworkStream_GetAccountCharacterSlotDataz = (fnCPythonNetworkStream_GetAccountCharacterSlotDataz)NewDetour("55 8b ec 8b 55 08 83 fa 05 72 09 a1 ?? ?? ?? ?? 5d c2 08 00");
	CPythonNetworkStream_GetAccountCharacterSlotDatau = (fnCPythonNetworkStream_GetAccountCharacterSlotDatau)NewDetour("55 8b ec 8b 55 08 56 83 fa 05");
	WCHEAT_assert(CPythonNetworkStream_RecvWhisperPacket);
	WCHEAT_assert(CPythonNetworkStream_SendWhisperPacket);
	WCHEAT_assert(CPythonNetworkStream_GetAccountCharacterSlotDataz);
	WCHEAT_assert(CPythonNetworkStream_GetAccountCharacterSlotDatau);
}
