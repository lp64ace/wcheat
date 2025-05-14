#ifndef CPYTHON_NETWORK_STREAM_HH
#define CPYTHON_NETWORK_STREAM_HH

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/** \name Types
 * \{ */

enum {
	ACCOUNT_CHARACTER_SLOT_ID,
	ACCOUNT_CHARACTER_SLOT_NAME,
	ACCOUNT_CHARACTER_SLOT_RACE,
	ACCOUNT_CHARACTER_SLOT_LEVEL,
	ACCOUNT_CHARACTER_SLOT_STR,
	ACCOUNT_CHARACTER_SLOT_DEX,
	ACCOUNT_CHARACTER_SLOT_HTH,
	ACCOUNT_CHARACTER_SLOT_INT,
	ACCOUNT_CHARACTER_SLOT_PLAYTIME,
	ACCOUNT_CHARACTER_SLOT_FORM,
	ACCOUNT_CHARACTER_SLOT_ADDR,
	ACCOUNT_CHARACTER_SLOT_PORT,
	ACCOUNT_CHARACTER_SLOT_GUILD_ID,
	ACCOUNT_CHARACTER_SLOT_GUILD_NAME,
	ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG,
	ACCOUNT_CHARACTER_SLOT_HAIR,
};

/** \} */

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

typedef struct CPythonNetworkStream CPythonNetworkStream;

typedef bool(__thiscall *fnCPythonNetworkStream_RecvWhisperPacket)(void *me);
extern fnCPythonNetworkStream_RecvWhisperPacket CPythonNetworkStream_RecvWhisperPacket;
typedef bool(__thiscall *fnCPythonNetworkStream_SendWhisperPacket)(void *me, const char *name, const char *c_szChat);
extern fnCPythonNetworkStream_SendWhisperPacket CPythonNetworkStream_SendWhisperPacket;
typedef const char *(__thiscall *fnCPythonNetworkStream_GetAccountCharacterSlotDataz)(void *me, unsigned int iSlot, unsigned int eType);
extern fnCPythonNetworkStream_GetAccountCharacterSlotDataz CPythonNetworkStream_GetAccountCharacterSlotDataz;
typedef unsigned int (__thiscall *fnCPythonNetworkStream_GetAccountCharacterSlotDatau)(void *me, unsigned int iSlot, unsigned int eType);
extern fnCPythonNetworkStream_GetAccountCharacterSlotDatau CPythonNetworkStream_GetAccountCharacterSlotDatau;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Offsets
 * \{ */

/**
 * This is not an actual function as it is optimized out of the compiler and converted into a static memory location!
 */
struct CPythonNetworkStream *CPythonNetworkStream_Instance();

uint32_t *CPythonNetworkStream_mSelectedCharacterIndex(CPythonNetworkStream *me);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Util
 * \{ */

void CPythonNetworkStream_Detour();

/** \} */

#ifdef __cplusplus
}
#endif

#endif // CPYTHON_NETWORK_STREAM_HH
