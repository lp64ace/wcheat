#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "CPythonChat.hh"
#include "CPythonNetworkStream.hh"

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

fnCPythonChat_AppendWhisper CPythonChat_AppendWhisper = NULL;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

/** \} */

void CPythonChat_Detour() {
	CPythonChat_AppendWhisper = (fnCPythonChat_AppendWhisper)NewDetourThisCall(
		"55 8b ec 83 ec 30 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b c1 c7 45 f4",
		[](void *me, int iType, const char *c_szName, const char *c_szChat) -> void {
			CPythonNetworkStream *nkNetStream = CPythonNetworkStream_Instance();
			unsigned int uSlot = *CPythonNetworkStream_mSelectedCharacterIndex(nkNetStream);
			const char *c_szPlayer = CPythonNetworkStream_GetAccountCharacterSlotDataz(nkNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_NAME);
			unsigned int uLevel = CPythonNetworkStream_GetAccountCharacterSlotDatau(nkNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_LEVEL);
			Logf("ACC %u:%s[%u] CHAT %s MSG %s", uSlot, c_szPlayer, uLevel, c_szName, c_szChat);
			CPythonChat_AppendWhisper(me, iType, c_szName, c_szChat);
		}
	);
	WCHEAT_assert(CPythonChat_AppendWhisper);
}
