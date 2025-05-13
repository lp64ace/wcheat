#ifndef NDEBUG
#	include "MEM_guardedalloc.h"
#endif

#include "LIB_string.h"
#include "LIB_utildefines.h"

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

int main(int argc, char **argv) {
#ifndef NDEBUG
	MEM_init_memleak_detection();
	MEM_enable_fail_on_memleak();
	MEM_use_guarded_allocator();
#endif

	if (argc < 3) {
		fprintf(stderr, "[Error] Incorrect usage!\n");
		fprintf(stderr, "[Error] \t%s <process> <dll>\n", argv[0]);
		return 0xdeadbeef;
	}

	const char *me = argv[0];
	const char *who = argv[1];
	const char *dll = argv[2];
	
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "[Error] Failed to get process list!\n");
		return 0xdeadbeef;
	}

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);
	
	Process32First(snapshot, &entry);
	do {
		if (_stricmp(entry.szExeFile, who) == 0) {
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
			if (!hProcess) {
				fprintf(stderr, "[Error] Failed to get process handle!\n");
				return 0xdeadbeef;
			}
			LPVOID lpRemoteMem = VirtualAllocEx(hProcess, NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
			if (!lpRemoteMem) {
				fprintf(stderr, "[Error] Failed to allocate process memory!\n");
				return 0xdeadbeef;
			}
			WriteProcessMemory(hProcess, lpRemoteMem, dll, LIB_strlen(dll) + 1, NULL);
			HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, lpRemoteMem, 0, NULL);
			if (!hThread) {
				fprintf(stderr, "[Error] Failed to create remote thread!\n");
				return 0xdeadbeef;
			}
			DWORD dwResult = 0;
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &dwResult);
			if (!dwResult) {
				fprintf(stderr, "[Error] Failed to inject DLL!\n");
				// return 0xdeadbeef;
			}
			CloseHandle(hThread);
			VirtualFreeEx(hProcess, lpRemoteMem, 0, MEM_RELEASE);
			CloseHandle(hProcess);
		}
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);

	return 0;
}
