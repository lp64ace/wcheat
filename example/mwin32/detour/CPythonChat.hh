#ifndef CPYTHON_CHAT_HH
#define CPYTHON_CHAT_HH

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

typedef struct CPythonChat CPythonChat;

typedef void (__thiscall *fnCPythonChat_AppendWhisper)(void *me, int iType, const char *c_szName, const char *c_szChat);
extern fnCPythonChat_AppendWhisper CPythonChat_AppendWhisper;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

/** \} */

/* -------------------------------------------------------------------- */
/** \name Util
 * \{ */

void CPythonChat_Attach(void *pLocation, size_t dwLength);
void CPythonChat_Detach();

/** \} */

#ifdef __cplusplus
}
#endif

#endif // CPYTHON_CHAT_HH