#ifndef CPYTHON_APPLICATION_HH
#define CPYTHON_APPLICATION_HH

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

typedef struct CPythonApplication CPythonApplication;

typedef void(__thiscall *fnCPythonApplication_UpdateGame)(void *me);
extern fnCPythonApplication_UpdateGame CPythonApplication_UpdateGame;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

void *CPythonApplication_mNetworkStream(CPythonApplication *me);
void *CPythonApplication_mChat(CPythonApplication *me);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Util
 * \{ */

void CPythonApplication_Attach(void *pLocation, size_t dwLength);
void CPythonApplication_Detach();

/** \} */

#ifdef __cplusplus
}
#endif

#endif // CPYTHON_APPLICATION_HH