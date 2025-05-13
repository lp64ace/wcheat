#include "LIB_memory.hh"
#include "LIB_utildefines.h"

void *vCPythonChat_AppendWhisper = NULL;

WCHEAT_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID unused) {
	switch (reason) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);
			OnProcessAttach(hModule, unused);

			/**
			 * This hooks the function 'CPythonChat::AppendWhisper' from the metin2client.exe executable, from the Raventor_V2 implementation.
			 */
			vCPythonChat_AppendWhisper = NewDetourThis("55 8b ec 83 ec 30 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b c1 c7 45 f4",
			(std::function<void (void *, int, const char *, const char *)>)[](void *me, int iType, const char *c_szName, const char *c_szChat) -> void {
				auto fn = reinterpret_cast<void(__thiscall *)(void *me, int, const char *, const char *)>(vCPythonChat_AppendWhisper);
				fprintf(stdout, "[Chat] PTR %p NAME %s MSG %s\n", me, c_szName, c_szChat);
				fn(me, iType, c_szName, c_szChat);
			});
		} break;
		case DLL_PROCESS_DETACH: {
			OnProcessDetach(hModule, unused);
		} break;
	}
	return TRUE;
}
