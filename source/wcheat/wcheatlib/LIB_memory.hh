#ifndef LIB_MEMORY_HH
#define LIB_MEMORY_HH

#include "LIB_memory.h"

#include <windows.h>
#include <detours.h>
#include <psapi.h>
#include <stdio.h>

#include <functional>
#include <memory>

enum {
	ATTACH_CDECL,
	ATTACH_FASTCALL,
	ATTACH_THISCALL,
};

void OnProcessAttach(HMODULE module, LPVOID unused);
void OnProcessDetach(HMODULE module, LPVOID unused);

template<uintptr_t Id, typename Ret, typename... Args>
class DetourCDeclThunk {
public:
	using FnType = Ret(__cdecl *)(Args...);

	static Ret __cdecl Detour(Args... args) {
		return (*m_Lambda)(args...);
	}

	static FnType Create(std::shared_ptr<std::function<Ret(Args...)>> lambda) {
		m_Lambda = lambda;
		return &Detour;
	}

private:
	static std::shared_ptr<std::function<Ret(Args...)>> m_Lambda;
};

template<uintptr_t Id, typename Ret, typename... Args>
std::shared_ptr<std::function<Ret(Args...)>> DetourCDeclThunk<Id, Ret, Args...>::m_Lambda;

template<uintptr_t Id, typename Ret, typename... Args>
class DetourFastCallThunk {
public:
	using FnType = Ret(__fastcall *)(Args...);

	static Ret __fastcall Detour(Args... args) {
		return (*m_Lambda)(args...);
	}

	static FnType Create(std::shared_ptr<std::function<Ret(Args...)>> lambda) {
		m_Lambda = lambda;
		return &Detour;
	}

private:
	static std::shared_ptr<std::function<Ret(Args...)>> m_Lambda;
};

template<uintptr_t Id, typename Ret, typename... Args>
std::shared_ptr<std::function<Ret(Args...)>> DetourFastCallThunk<Id, Ret, Args...>::m_Lambda;

template<uintptr_t Id, typename Ret, typename... Args>
class DetourThisCallThunk {
public:
	using FnType = Ret(__thiscall *)(Args...);

	static Ret __thiscall Detour(Args... args) {
		return (*m_Lambda)(args...);
	}

	static FnType Create(std::shared_ptr<std::function<Ret(Args...)>> lambda) {
		m_Lambda = lambda;
		return &Detour;
	}

private:
	static std::shared_ptr<std::function<Ret(Args...)>> m_Lambda;
};

template<uintptr_t Id, typename Ret, typename... Args>
std::shared_ptr<std::function<Ret(Args...)>> DetourThisCallThunk<Id, Ret, Args...>::m_Lambda;

void* DoDetour(void* pLocation, void* pDetour);

template<typename Ret, typename... Args> void *NewDetourEx(void *pLocation, std::function<Ret(Args...)> lambda) {
	return DoDetour(pLocation, (void*)DetourCDeclThunk<__COUNTER__, Ret, Args...>::Create(std::make_shared< std::function<Ret(Args...)>>(std::move(lambda))));
}

template<typename Ret, typename... Args> void* NewDetourFastEx(void* pLocation, std::function<Ret(Args...)> lambda) {
	return DoDetour(pLocation, (void*)DetourFastCallThunk<__COUNTER__, Ret, Args...>::Create(std::make_shared< std::function<Ret(Args...)>>(std::move(lambda))));
}

template<typename Ret, typename... Args> void* NewDetourThisEx(void* pLocation, std::function<Ret(Args...)> lambda) {
	return DoDetour(pLocation, (void*)DetourThisCallThunk<__COUNTER__, Ret, Args...>::Create(std::make_shared< std::function<Ret(Args...)>>(std::move(lambda))));
}

template<typename Lambda> void *NewDetour(const char *pattern, Lambda &&lambda) {
	void *pLocation = NULL;
	
	MODULEINFO info;
	if (GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO))) {
		if (!(pLocation = (void *)LIB_pattern_offset(GetModuleHandle(NULL), info.SizeOfImage, pattern))) {
			return NULL;
		}
	}
	
	return NewDetourThisEx(pLocation, std::function(std::forward<Lambda>(lambda)));
}

template<typename Lambda> void *NewDetourFast(const char *pattern, Lambda &&lambda) {
	void* pLocation = NULL;

	MODULEINFO info;
	if (GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO))) {
		if (!(pLocation = (void*)LIB_pattern_offset(GetModuleHandle(NULL), info.SizeOfImage, pattern))) {
			return NULL;
		}
	}

	return NewDetourThisEx(pLocation, std::function(std::forward<Lambda>(lambda)));
}

template<typename Lambda> void *NewDetourThis(const char *pattern, Lambda &&lambda) {
	void* pLocation = NULL;

	MODULEINFO info;
	if (GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO))) {
		if (!(pLocation = (void*)LIB_pattern_offset(GetModuleHandle(NULL), info.SizeOfImage, pattern))) {
			return NULL;
		}
	}

	return NewDetourThisEx(pLocation, std::function(std::forward<Lambda>(lambda)));
}

#endif
