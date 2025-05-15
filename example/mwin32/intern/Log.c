#include "MEM_guardedalloc.h"

#include "LIB_string.h"

#include "Log.h"

#include <stdio.h>
#include <windows.h>

#define CONNECTING_STATE 0
#define READING_STATE 1
#define WRITING_STATE 2
#define INSTANCES 32

typedef struct {
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;
	BOOL bConnected;
} PIPEINST, *LPPIPEINST; 

PIPEINST Pipe[INSTANCES];
HANDLE Event[INSTANCES]; 

void LogSpawn() {
	for (DWORD dwIndex = 0; dwIndex < INSTANCES; dwIndex++) {
		Event[dwIndex] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!Event[dwIndex]) {
			printf("CreateEvent failed with %lu\n", GetLastError());
			continue;
		}

		ZeroMemory(&Pipe[dwIndex].oOverlap, sizeof(OVERLAPPED));
		Pipe[dwIndex].oOverlap.hEvent = Event[dwIndex];
		Pipe[dwIndex].bConnected = FALSE;

		Pipe[dwIndex].hPipeInst = CreateNamedPipeA("\\\\.\\pipe\\mwin32.pipe", PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, INSTANCES, 0, 1024, 0, NULL);
		if (Pipe[dwIndex].hPipeInst == INVALID_HANDLE_VALUE) {
			printf("CreateNamedPipe failed with %lu\n", GetLastError());
			CloseHandle(Event[dwIndex]);
			continue;
		}

		BOOL connected = ConnectNamedPipe(Pipe[dwIndex].hPipeInst, &Pipe[dwIndex].oOverlap);
		DWORD err = GetLastError();

		if (!connected && err == ERROR_IO_PENDING) {
			// Async connect in progress
		}
		else if (!connected && err == ERROR_PIPE_CONNECTED) {
			// Client is already connected
			SetEvent(Event[dwIndex]);
			Pipe[dwIndex].bConnected = TRUE;
		}
		else if (!connected) {
			printf("ConnectNamedPipe failed with %lu\n", err);
			CloseHandle(Pipe[dwIndex].hPipeInst);
			CloseHandle(Event[dwIndex]);
		}
	}
}
bool LogPoll(char *szText, size_t dwMaxLength) {
	DWORD dwBytesRead;
	BOOL fSuccess;
	for (DWORD i = 0; i < INSTANCES; i++) {
		if (!Pipe[i].bConnected) {
			// Check if connection is complete
			DWORD dwRes = WaitForSingleObject(Event[i], 0);
			if (dwRes == WAIT_OBJECT_0) {
				Pipe[i].bConnected = TRUE;
				ResetEvent(Event[i]);
			}
		}

		if (Pipe[i].bConnected) {
			ZeroMemory(&Pipe[i].oOverlap, sizeof(OVERLAPPED));
			Pipe[i].oOverlap.hEvent = Event[i];

			fSuccess = ReadFile(Pipe[i].hPipeInst, szText, dwMaxLength, &dwBytesRead, &Pipe[i].oOverlap);

			if (!fSuccess && GetLastError() == ERROR_IO_PENDING) {
				DWORD dwRes = WaitForSingleObject(Event[i], 0);
				if (dwRes == WAIT_OBJECT_0) {
					GetOverlappedResult(Pipe[i].hPipeInst, &Pipe[i].oOverlap, &dwBytesRead, FALSE);
					if (dwBytesRead > 0) {
						szText[min(dwMaxLength - 1, (size_t)dwBytesRead)] = '\0';
						ResetEvent(Event[i]);
						return true;
					}
				}
			}
			else if (fSuccess && dwBytesRead > 0) {
				szText[min(dwMaxLength - 1, (size_t)dwBytesRead)] = '\0';
				return true;
			}
			else {
				DWORD err = GetLastError();
				if (err == ERROR_BROKEN_PIPE || err == ERROR_NO_DATA) {
					// Client disconnected; clean and reinitialize this pipe
					DisconnectNamedPipe(Pipe[i].hPipeInst);
					CloseHandle(Pipe[i].hPipeInst);
					Pipe[i].hPipeInst = CreateNamedPipeA("\\\\.\\pipe\\mwin32.pipe", PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, INSTANCES, 0, 1024, 0, NULL);
					Pipe[i].bConnected = FALSE;
					ConnectNamedPipe(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
				}
			}
		}
	}

	return false;
}

HANDLE LogConnect() {
	return CreateFile("\\\\.\\pipe\\mwin32.pipe", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}
void LogDisconnect(HANDLE hPipe) {
	if (hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(hPipe);
	}
}

void Log(const char *msg) {
	HANDLE hPipe = LogConnect();
	if (hPipe != INVALID_HANDLE_VALUE) {
		char szPid[16];

		DWORD dwLength = 0;
		WriteFile(hPipe, msg, (DWORD)strlen(msg), &dwLength, NULL);
		FlushFileBuffers(hPipe);
		LogDisconnect(hPipe);
	}
}

void Logf(ATTR_PRINTF_FORMAT const char *msg, ...) {
	char *szBuffer = MEM_mallocN(0xffffu, "Log");

	va_list args;
	va_start(args, msg);
	(void)LIB_vstrnformat(szBuffer, 0xffffu, msg, args);
	Log(szBuffer);
	va_end(args);

	MEM_freeN(szBuffer);
}
