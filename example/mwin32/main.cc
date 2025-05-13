#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "intern/Log.h"

using fnCPythonChat_AppendWhisper = void(__thiscall *)(void *me, int, const char *, const char *);
void *CPythonChat_AppendWhisper = NULL;

WCHEAT_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID unused) {
	switch (reason) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);
			OnProcessAttach(hModule, unused);

			Log("Hello!");

			CPythonChat_AppendWhisper = NewDetourThis("55 8b ec 83 ec 30 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b c1 c7 45 f4", [](void *me, int iType, const char *c_szName, const char *c_szChat) -> void {
				auto fn = reinterpret_cast<fnCPythonChat_AppendWhisper>(CPythonChat_AppendWhisper);
				Logf("CHAT %p NAME %s MSG %s", me, c_szName, c_szChat);
				fn(me, iType, c_szName, c_szChat);
			});
			WCHEAT_assert(CPythonChat_AppendWhisper);
		} break;
		case DLL_PROCESS_DETACH: {
			Log("Bye!");

			OnProcessDetach(hModule, unused);
		} break;
	}
	return TRUE;
}
