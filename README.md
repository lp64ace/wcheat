# WCHEAT

Create advanced function hooks for Windows applications.
This project leverages the Microsoft Detours library and pattern-matching techniques to enable easy and 
reliable function hooking—commonly used in reverse engineering, debugging, and game modding.

## How to Build

### Prerequisites

Before proceeding, ensure that the following prerequisites are met:

- **Git:** Make sure Git is installed on your system.
- **CMake:** Ensure CMake is installed and accessible from your command prompt.

### Building WCheat

1. Clone the WCheat repository:

```bash
git clone https://github.com/lp64ace/wcheat.git
cd wcheat
```

2. Compile the project, and enjoy!

```bash
cmake --build .
```

### Examples

Here is a quick example from the test directory working on 5/2025 for the metin2 client implementation of the Raventor_V2 server.

```cpp
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
```

# Authors

Dimitris Bokis,
undergraduate student at Aristotle University of Thessaloniki
