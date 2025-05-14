#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "detour/CPythonChat.hh"
#include "detour/CPythonNetworkStream.hh"

WCHEAT_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID unused) {
	switch (reason) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);
			OnProcessAttach(hModule, unused);

			CPythonChat_Detour();
			CPythonNetworkStream_Detour();
		} break;
		case DLL_PROCESS_DETACH: {
			OnProcessDetach(hModule, unused);
		} break;
	}
	return TRUE;
}
