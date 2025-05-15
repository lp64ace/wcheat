#ifndef IABSTRACT_PLAYER_HH
#define IABSTRACT_PLAYER_HH

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

typedef struct IAbstractPlayer IAbstractPlayer;

/**
 * This is not an actual function as it is optimized out of the compiler and converted into a static memory location!
 */
struct IAbstractPlayer *IAbstractPlayer_GetSingleton();

/** \} */

/* -------------------------------------------------------------------- */
/** \name Util
 * \{ */

void IAbstractPlayer_Attach(void *pLocation, size_t dwLength);
void IAbstractPlayer_Detach();

/** \} */

#ifdef __cplusplus
}
#endif

#endif
