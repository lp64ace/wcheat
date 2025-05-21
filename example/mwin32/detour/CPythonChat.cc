#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "CPythonChat.hh"
#include "CPythonEventManager.hh"
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

void CPythonChat_Attach(void *pLocation, size_t dwLength) {
	CPythonChat_AppendWhisper = (fnCPythonChat_AppendWhisper)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 83 ec 30 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b c1 c7 45 f4");
	ALWAYS_ASSERT(CPythonChat_AppendWhisper);
}

void CPythonChat_Detach() {
}
