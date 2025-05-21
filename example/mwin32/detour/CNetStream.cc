#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "CNetStream.hh"

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

fnCNetworkStream_Recv CNetworkStream_Recv = NULL;
fnCNetworkStream_Peek CNetworkStream_Peek = NULL;
fnCNetworkStream_Send CNetworkStream_Send = NULL;

/** \} */

void CNetStream_Attach(void *pLocation, size_t dwLength) {
	CNetworkStream_Recv = (fnCNetworkStream_Recv)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 56 57 ff 75 0c 8b 7d 08");
	CNetworkStream_Peek = (fnCNetworkStream_Peek)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 56 8b f1 e8 55 ff ff ff 8b 4d 08");
	CNetworkStream_Send = (fnCNetworkStream_Send)LIB_pattern_offset(pLocation, dwLength, "55 8b ec 56 8b f1 57 8b 7d 08 8b 56 30 8b");
	ALWAYS_ASSERT(CNetworkStream_Recv);
	ALWAYS_ASSERT(CNetworkStream_Peek);
	ALWAYS_ASSERT(CNetworkStream_Send);
}

void CNetStream_Detach() {
}

