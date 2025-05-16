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
typedef bool(__thiscall *fnCPythonNetworkStream_SendWhisperPacket)(void *me, const char *name, const char *c_szChat);
extern fnCPythonNetworkStream_RecvWhisperPacket CPythonNetworkStream_RecvWhisperPacket;
extern fnCPythonNetworkStream_SendWhisperPacket CPythonNetworkStream_SendWhisperPacket;

typedef bool(__thiscall *fnCPythonNetworkStream_CheckPacket)(void *me, unsigned char *bHeader);
extern fnCPythonNetworkStream_CheckPacket CPythonNetworkStream_CheckPacket;

typedef const char *(__thiscall *fnCPythonNetworkStream_GetAccountCharacterSlotDataz)(void *me, unsigned int iSlot, unsigned int eType);
typedef unsigned int(__thiscall *fnCPythonNetworkStream_GetAccountCharacterSlotDatau)(void *me, unsigned int iSlot, unsigned int eType);
extern fnCPythonNetworkStream_GetAccountCharacterSlotDataz CPythonNetworkStream_GetAccountCharacterSlotDataz;
extern fnCPythonNetworkStream_GetAccountCharacterSlotDatau CPythonNetworkStream_GetAccountCharacterSlotDatau;

typedef bool(__thiscall *fnCPythonNetworkStream_SendScriptAnswerPacket)(void *me, int iAnswer);
typedef bool(__thiscall *fnCPythonNetworkStream_SendScriptButtonPacket)(void *me, unsigned int uIndex);
typedef bool(__thiscall *fnCPythonNetworkStream_SendQuestInputStringPacket)(void *me, const char *c_szString);
typedef bool(__thiscall *fnCPythonNetworkStream_SendQuestConfirmPacket)(void *me, unsigned char bAnswer, unsigned int uPID);
extern fnCPythonNetworkStream_SendScriptAnswerPacket CPythonNetworkStream_SendScriptAnswerPacket;
extern fnCPythonNetworkStream_SendScriptButtonPacket CPythonNetworkStream_SendScriptButtonPacket;
extern fnCPythonNetworkStream_SendQuestInputStringPacket CPythonNetworkStream_SendQuestInputStringPacket;
extern fnCPythonNetworkStream_SendQuestConfirmPacket CPythonNetworkStream_SendQuestConfirmPacket;

typedef void(__thiscall *fnCPythonNetworkStream_OnScriptEventStart)(void *me, int iSkin, int iIndex);
extern fnCPythonNetworkStream_OnScriptEventStart CPythonNetworkStream_OnScriptEventStart;

typedef bool(__thiscall *fnCPythonNetworkStream_RecvScriptPacket)(void *me);
typedef bool(__thiscall *fnCPythonNetworkStream_RecvQuestInfoPacket)(void *me);
typedef void(__thiscall *fnCPythonNetworkStream_RecvQuestConfirmPacket)(void *me);
extern fnCPythonNetworkStream_RecvScriptPacket CPythonNetworkStream_RecvScriptPacket;
extern fnCPythonNetworkStream_RecvQuestInfoPacket CPythonNetworkStream_RecvQuestInfoPacket;
extern fnCPythonNetworkStream_RecvQuestConfirmPacket CPythonNetworkStream_RecvQuestConfirmPacket;

typedef void(__thiscall *fnCPythonNetworkStream___RefreshInventoryWindow)(void *me);
extern fnCPythonNetworkStream___RefreshInventoryWindow CPythonNetworkStream___RefreshInventoryWindow;

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

void CPythonNetworkStream_Attach(void *pLocation, size_t dwLength);
void CPythonNetworkStream_Detach();

/** \} */

#ifdef __cplusplus
}
#endif

#endif // CPYTHON_NETWORK_STREAM_HH
