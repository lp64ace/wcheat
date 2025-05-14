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
