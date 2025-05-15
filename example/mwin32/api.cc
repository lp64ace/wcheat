#include "MEM_guardedalloc.h"

#include "LIB_string.h"
#include "LIB_utildefines.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "intern/Log.h"

extern "C" const int datatoc_mwin32_dll_size;
extern "C" const char datatoc_mwin32_dll[];

CHAR szDLL[MAX_PATH];

std::vector<std::string> mLogs;

HMODULE GetRemoteModuleHandle(DWORD pid, const char *dll) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	MODULEENTRY32 me = {sizeof(me)};
	Module32First(snapshot, &me);
	do {
		if (_stricmp(me.szModule, dll) == 0) {
			CloseHandle(snapshot);
			return me.hModule;
		}
	} while (Module32Next(snapshot, &me));

	CloseHandle(snapshot);
	return NULL;
}

void ShowConsoleCursor(BOOL bShow) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = bShow;
	SetConsoleCursorInfo(hConsole, &info);
}

void GotoXY(short x, short y) {
	COORD dwCoord = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), dwCoord);
}

void ClearConsoleLine(short y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return;
	COORD dwStart = {0, y};
	DWORD dwWrite;
	FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X, dwStart, &dwWrite);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X, dwStart, &dwWrite);
}

int ForEach(std::function<void (DWORD pid)> callback) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return 0xdeadbeef;
	}

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);
	
	Process32First(snapshot, &entry);
	do {
		if (_stricmp(entry.szExeFile, "metin2client.exe") == 0) {
			callback(entry.th32ProcessID);
		}
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);
	
	return 0;
}

void Inject(DWORD pid) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		Log("[Error] Failed to get process handle!");
		return;
	}
	do {
		LPVOID lpRemoteMem = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);
		if (!lpRemoteMem) {
			Log("[Error] Failed to allocate process memory!");
			return;
		}
		WriteProcessMemory(hProcess, lpRemoteMem, szDLL, MAX_PATH, NULL);
		do {
			HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, lpRemoteMem, 0, NULL);
			if (!hThread) {
				Log("[Error] Failed to create remote thread!");
				break;
			}
			DWORD dwResult = 0;
			while ((dwResult = WaitForSingleObject(hThread, 0))) {
				char szBuffer[512];
				if (LogPoll(szBuffer, ARRAY_SIZE(szBuffer))) {
					mLogs.push_back(szBuffer);
				}
			}
			GetExitCodeThread(hThread, &dwResult);
			CloseHandle(hThread);
			if (!dwResult) {
				Log("[Error] Failed to inject DLL!");
				break;
			}
		} while (false);
		VirtualFreeEx(hProcess, lpRemoteMem, 0, MEM_RELEASE);
	} while (false);
	CloseHandle(hProcess);

	if (!GetRemoteModuleHandle(pid, "mwin32.dll")) {
		Log("[Error] Failed to inject DLL! (Unkown)");
	}
}

void Eject(DWORD pid) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		Log("[Error] Failed to get process handle!");
		return;
	}
	do {
		HMODULE hRemoteLibrary = GetRemoteModuleHandle(pid, "mwin32.dll");
		if (!hRemoteLibrary) {
			Log("[Error] Failed to find loaded DLL!");
			break;
		}
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, hRemoteLibrary, 0, NULL);
		if (!hThread) {
			Log("[Error] Failed to create remote thread!");
			break;
		}
		DWORD dwResult = 0;
		while ((dwResult = WaitForSingleObject(hThread, 0))) {
			char szBuffer[512];
			if (LogPoll(szBuffer, ARRAY_SIZE(szBuffer))) {
				mLogs.push_back(szBuffer);
			}
		}
		GetExitCodeThread(hThread, &dwResult);
		if (!dwResult) {
			Log("[Error] Failed to unload DLL!");
			break;
		}
		CloseHandle(hThread);
	} while (false);
	CloseHandle(hProcess);
}

void UpdateDLL() {
	CHAR szDirectory[MAX_PATH];
	if (GetCurrentDirectory(MAX_PATH, szDirectory)) {
		do {
			HANDLE hFile = CreateFile("mwin32.dll", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				break;
			}
			DWORD dwLength = 0;
			if (!WriteFile(hFile, datatoc_mwin32_dll, datatoc_mwin32_dll_size, &dwLength, NULL)) {
				CloseHandle(hFile);
				break;
			}
			CloseHandle(hFile);
		} while (false);

		PathCombine(szDLL, szDirectory, "mwin32.dll");
	}
}

int main(int argc, char **argv) {
#ifndef NDEBUG
	MEM_init_memleak_detection();
	MEM_enable_fail_on_memleak();
	MEM_use_guarded_allocator();
#endif

	ShowConsoleCursor(FALSE);
	SetConsoleOutputCP(1253);
	LogSpawn();

	std::vector<DWORD> view;
	
	for (uintptr_t line = 0; line < 32; line++) {
		ClearConsoleLine(line);
	}

	ptrdiff_t selected = 0;
	while (true) {
		uintptr_t line;

		view.clear();
		ForEach([&](DWORD pid) -> void {
			view.push_back(pid);
		});

		GotoXY(0, 0);
		ClearConsoleLine((line = 0)++);
		fprintf(stdout, "[VIEW]\n");
		
		for (ptrdiff_t index = 0; index < 8; ++index) {
			ClearConsoleLine(line++);
		}

		for (ptrdiff_t index = WCHEAT_MAX(0, selected - 4); index < WCHEAT_MIN(view.size(), WCHEAT_MAX(0, selected - 4) + 8); ++index) {
			if (0 <= index && index < view.size()) {
				DWORD pid = view[index];
				fprintf(stdout, "%c[PID %5lu] STATUS [%c]\n", " >"[(index == selected)], pid, " X"[GetRemoteModuleHandle(pid, "mwin32.dll") != NULL]);
			}
		}

		GotoXY(0, 8);
		ClearConsoleLine((line = 8)++);
		fprintf(stdout, "[LOGS]\n");

		line = 9;

		for (ptrdiff_t index = WCHEAT_MAX(0, (ptrdiff_t)mLogs.size() - 8); index < mLogs.size(); ++index) {
			ClearConsoleLine(line++);
		}

		for (ptrdiff_t index = WCHEAT_MAX(0, (ptrdiff_t)mLogs.size() - 8); index < mLogs.size(); ++index) {
			fprintf(stdout, "%5zd | %s\n", index, &mLogs[index][0]);
		}

		Sleep(16);

		if (kbhit()) {
			switch (getch()) {
				case 32: {	// SPACE
					if (view.size()) {
						DWORD pid = view[WCHEAT_MIN(WCHEAT_MAX(0, selected), view.size() - 1)];

						if (GetRemoteModuleHandle(pid, "mwin32.dll")) {
							Eject(pid);
							UpdateDLL();
						}
						else {
							UpdateDLL();
							Inject(pid);
						}
					}
				} break;
				case 72: {	// UP
					selected = WCHEAT_MIN(WCHEAT_MAX(0, selected - 1), view.size() - 1);
				} break;
				case 80: {	// UP
					selected = WCHEAT_MIN(WCHEAT_MAX(0, selected + 1), view.size() - 1);
				} break;
				case 27: {
					return 0;
				} break;
			}
		}

		char *szBuffer = (char *)MEM_mallocN(0xffffu, "Log::Buffer");
		while (LogPoll(szBuffer, 0xffffu)) {
			mLogs.push_back(szBuffer);
		}
		MEM_freeN(szBuffer);
	}

	return 0;
}
