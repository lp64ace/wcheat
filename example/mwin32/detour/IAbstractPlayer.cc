#include "MEM_guardedalloc.h"

#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "IAbstractPlayer.hh"

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

/**
 * This is not an actual function as it is optimized out of the compiler and converted into a static memory location!
 */
IAbstractPlayer *IAbstractPlayer_GetSingleton() {
	void *lpPlayer = NULL;
	if (!lpPlayer) {
		/**
		 * The pattern locates a sequence of bytes in memory, and the resulting address points
		 * just after the end of this byte sequence. This address corresponds to a static memory
		 * block holding a pointer to a pointer (since it's a reference in C++).
		 */
		HMODULE hModule = GetModuleHandle(NULL);
		lpPlayer = (void *)LIB_pattern_offset(hModule, DetourGetModuleSize(hModule), "fc 33 cd e8 ?? ?? ?? ?? 8b e5 5d c3 ff 75 c4 8b 3d");
		ALWAYS_ASSERT(lpPlayer);
	}
	return **(IAbstractPlayer ***)POINTER_OFFSET(lpPlayer, 17 /**The number of bytes in the pattern */);
}

/** \} */

void IAbstractPlayer_Attach(void *pLocation, size_t dwLength) {
	(void)IAbstractPlayer_GetSingleton();
}
void IAbstractPlayer_Detach() {
}
