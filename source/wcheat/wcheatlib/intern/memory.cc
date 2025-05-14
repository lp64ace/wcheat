#include "LIB_memory.hh"

#include <mutex>
#include <vector>

#pragma comment(lib, "psapi.lib")

std::vector<std::pair<void*, void*>> m_Detours;
std::mutex m_Mutex;

void OnProcessAttach(HMODULE module, LPVOID unused) {
}

void OnProcessDetach(HMODULE module, LPVOID unused) {
	m_Mutex.lock();
	for (const std::pair<void*, void*>& detour : m_Detours) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach((PVOID*)&detour.first, detour.second);
		DetourTransactionCommit();
	}
	m_Detours.clear();
	m_Mutex.unlock();
}

void* DoDetour(void* pLocation, void* pDetour) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&pLocation, pDetour);
	DetourTransactionCommit();

	m_Mutex.lock();
	m_Detours.push_back(std::make_pair(pLocation, pDetour));
	m_Mutex.unlock();
	return pLocation;
}

void *NewDetour(const char *pattern) {
	void* pLocation = NULL;

	MODULEINFO info;
	if (GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO))) {
		if (!(pLocation = (void*)LIB_pattern_offset(GetModuleHandle(NULL), info.SizeOfImage, pattern))) {
			return NULL;
		}
	}
	
	return pLocation;
}
