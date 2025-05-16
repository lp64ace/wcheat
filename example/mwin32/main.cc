#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

#include "detour/CNetStream.hh"
#include "detour/CPythonApplication.hh"
#include "detour/CPythonChat.hh"
#include "detour/CPythonEventManager.hh"
#include "detour/CPythonNetworkStream.hh"

WCHEAT_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID unused) {
	switch (reason) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);

			CNetStream_Attach(GetModuleHandle(NULL), DetourGetModuleSize(GetModuleHandle(NULL)));
			CPythonApplication_Attach(GetModuleHandle(NULL), DetourGetModuleSize(GetModuleHandle(NULL)));
			CPythonChat_Attach(GetModuleHandle(NULL), DetourGetModuleSize(GetModuleHandle(NULL)));
			CPythonEventManager_Attach(GetModuleHandle(NULL), DetourGetModuleSize(GetModuleHandle(NULL)));
			CPythonNetworkStream_Attach(GetModuleHandle(NULL), DetourGetModuleSize(GetModuleHandle(NULL)));

			CPythonNetworkStream *nkNetStream = CPythonNetworkStream_Instance();
			if (nkNetStream) {
				uint32_t uSlot = *CPythonNetworkStream_mSelectedCharacterIndex(nkNetStream);
				uint32_t uLevel = CPythonNetworkStream_GetAccountCharacterSlotDatau(nkNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_LEVEL);
				Logf("Hello, %u:%s[Lv %u]", uSlot, CPythonNetworkStream_GetAccountCharacterSlotDataz(nkNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_NAME), uLevel);
			}
		} break;
		case DLL_PROCESS_DETACH: {
			CPythonNetworkStream_Detach();
			CPythonEventManager_Detach();
			CPythonChat_Detach();
			CPythonApplication_Detach();
			CNetStream_Detach();
		} break;
	}
	return TRUE;
}
