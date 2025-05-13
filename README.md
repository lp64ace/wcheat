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

### Example: Hooking `CPythonChat::AppendWhisper`

This is a real-world example from the 'example' directory, verified as working as of May 2025, using the Raventor_V2 Metin2 client implementation.

```cpp
using fnCPythonChat_AppendWhisperC = void(__thiscall *)(void *me, int, const char *, const char *);
void *vCPythonChat_AppendWhisper = NULL;

WCHEAT_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID unused) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            OnProcessAttach(hModule, unused);

            vCPythonChat_AppendWhisper = NewDetourThis(
                "55 8b ec 83 ec 30 a1 ?? ?? ?? ?? 33 c5 89 45 fc 8b c1 c7 45 f4",
                [](void *me, int iType, const char *c_szName, const char *c_szChat) -> void {
                    auto fn = reinterpret_cast<fnCPythonChat_AppendWhisperC>(vCPythonChat_AppendWhisper);
                    fprintf(stdout, "[Chat] PTR %p NAME %s MSG %s\n", me, c_szName, c_szChat);
                    fn(me, iType, c_szName, c_szChat);
                }
            );
        } break;
        case DLL_PROCESS_DETACH: {
            OnProcessDetach(hModule, unused);
        } break;
    }
    return TRUE;
}
```

### DLL Injection and Ejection Utilities

This repository includes two utilities:

1. `load` - Injects a DLL via remote thread
2. `free` - Ejects a DLL from remote process

NOTE: These require administrator privileges to function properly.

### Injecting a DLL:

Since injecting a DLL is executed from the current working directory of the remote thread use absolute paths.
```
load metin2client.exe /absolute/path/to/your.dll
```

### Unloading a DLL:

When unloading a DLL the name of the DLL is matched so do not include the path.
```
free metin2client.exe your.dll
```

# Authors

Dimitris Bokis,
undergraduate student at Aristotle University of Thessaloniki
