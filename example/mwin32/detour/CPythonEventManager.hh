#ifndef CPYTHON_EVENT_MANAGER_HH
#define CPYTHON_EVENT_MANAGER_HH

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/** \name Types
 * \{ */

/** \} */

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

typedef struct CPythonEventManager CPythonEventManager;

typedef bool(__thiscall *fnCPythonEventManager_GetScriptEventIndex)(void *me, const char *c_szName, int *pEventPosition, int *pEventType);
typedef void(__thiscall *fnCPythonEventManager_ProcessEventSet)(void *me, void *pEventSet);
typedef void(__thiscall *fnCPythonEventManager_CheckEventSetIndex)(void *me, int iIndex);
typedef void(__thiscall *fnCPythonEventManager_SelectAnswer)(void *me, int iIndex, int iAnswer);
typedef void(__thiscall *fnCPythonEventManager_EndEventProcess)(void *me, int iIndex);
typedef void(__thiscall *fnCPythonEventManager_ClearEventSeti)(void *me, int iIndex);
extern fnCPythonEventManager_GetScriptEventIndex CPythonEventManager_GetScriptEventIndex;
extern fnCPythonEventManager_ProcessEventSet CPythonEventManager_ProcessEventSet;
extern fnCPythonEventManager_CheckEventSetIndex CPythonEventManager_CheckEventSetIndex;
extern fnCPythonEventManager_SelectAnswer CPythonEventManager_SelectAnswer;
extern fnCPythonEventManager_EndEventProcess CPythonEventManager_EndEventProcess;
extern fnCPythonEventManager_ClearEventSeti CPythonEventManager_ClearEventSeti;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

struct CPythonEventManager *CPythonEventManager_Instance();

void *CPythonEventManager_mPyInterface(CPythonEventManager *me);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Util
 * \{ */

void CPythonEventManager_Attach(void *pLocation, size_t dwLength);
void CPythonEventManager_Detach();

/** \} */

#ifdef __cplusplus
}
#endif

#endif // CPYTHON_EVENT_MANAGER_HH
