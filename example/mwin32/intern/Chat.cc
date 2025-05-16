#include "MEM_guardedalloc.h"

#include "LIB_listbase.h"
#include "LIB_string.h"
#include "LIB_utildefines.h"

#include "detour/CPythonChat.hh"
#include "detour/CPythonNetworkStream.hh"

#include "Chat.h"
#include "ChatMessage.h"
#include "Http.h"
#include "Log.h"

#include <mutex>

typedef struct Chat {
	struct Chat *prev, *next;

	char szFrom[64];
	void *pNetStream;

	ListBase mHistory;
} Chat;

ListBase Chats;

WCHEAT_STATIC bool ChatIsPlayer(const char *szNameFrom) {
	return !STREQ(szNameFrom, "[System]");
}
WCHEAT_STATIC bool ChatIsUtil(const char *szChat) {
	return STREQ(szChat, "|?whisper_renewal>|") || STREQ(szChat, "|?whisper_renewal<|") || STREQ(szChat, "");
}

Chat *Chat_Find(const char *szNameFrom) {
	LISTBASE_FOREACH(Chat *, chat, &Chats) {
		if (STREQ(chat->szFrom, szNameFrom)) {
			return chat;
		}
	}
	return NULL;
}

Chat *Chat_FindEnsure(const char *szNameFrom) {
	Chat *pChat = Chat_Find(szNameFrom);
	if (!pChat) {
		pChat = (Chat *)MEM_mallocN(sizeof(Chat), "Chat");
		LIB_strcpy(pChat->szFrom, sizeof(pChat->szFrom), szNameFrom);
		LIB_listbase_clear(&pChat->mHistory);
		LIB_addtail(&Chats, pChat);
	}
	return pChat;
}

void Chat_PostMessage(void *pNetworkStream, const char *szNameFrom, const char *szChat) {
	if (!ChatIsPlayer(szNameFrom) || ChatIsUtil(szChat)) {
		return;
	}
	Chat *pChat = Chat_FindEnsure(szNameFrom);
	if (pNetworkStream) {
		pChat->pNetStream = pNetworkStream;
	}

	ChatMessage *pChatMessage = (ChatMessage *)MEM_callocN(sizeof(ChatMessage), "ChatMessage");
	LIB_strcpy(pChatMessage->szName, sizeof(pChatMessage->szName), szNameFrom);
	LIB_strcpy(pChatMessage->szText, sizeof(pChatMessage->szText), szChat);
	pChatMessage->iState = MESSAGE_IN;
	pChatMessage->tTime = time(NULL);
	LIB_addtail(&pChat->mHistory, pChatMessage);
	Logf("NEW MESSAGE FROM %s CONTENT %s", szNameFrom, szChat);
}

void Chat_PollMessageSingleInbound(Chat *pChat, ChatMessage *pMessage) {
	CPythonNetworkStream *pNetStream = CPythonNetworkStream_Instance();
	if (pNetStream) {
		unsigned int uSlot = *CPythonNetworkStream_mSelectedCharacterIndex(pNetStream);
		const char *szName = CPythonNetworkStream_GetAccountCharacterSlotDataz(pNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_NAME);

		ChatMessage *pNewMessage = (ChatMessage *)MEM_callocN(sizeof(ChatMessage), "ChatMessage");
		LIB_strcpy(pNewMessage->szName, sizeof(pNewMessage->szName), szName);
		LIB_strcpy(pNewMessage->szText, sizeof(pNewMessage->szText), "");
		pNewMessage->iState = MESSAGE_GPT;
		pNewMessage->tTime = time(NULL);
		LIB_addtail(&pChat->mHistory, pNewMessage);
	}
}

void Chat_PollMessageSingleUploadGPT(void *pPythonChat, Chat *pChat, ChatMessage *pMessage) {
	if (difftime(time(NULL), pMessage->tTime) < 4) {
		return;
	}
	pMessage->tTime = time(NULL);
	pMessage->iState = MESSAGE_GET;
	CPythonNetworkStream_SendWhisperPacket(pChat->pNetStream, pChat->szFrom, "|?whisper_renewal>|");
}

void Chat_PollMessageSingleDownloadGPT(void *pPythonChat, Chat *pChat, ChatMessage *pMessage) {
	if (difftime(time(NULL), pMessage->tTime) < 2) {
		return;
	}
	pMessage->tTime = time(NULL);
	Http_Poll(pMessage);
}

void Chat_PollMessageSingleSend(void *pPythonChat, Chat *pChat, ChatMessage *pMessage) {
	pMessage->tTime = time(NULL);
	if (!strstr(pMessage->szText, "null")) {
		CPythonNetworkStream *pNetStream = CPythonNetworkStream_Instance();
		if (pNetStream) {
			unsigned int uSlot = *CPythonNetworkStream_mSelectedCharacterIndex(pNetStream);
			const char *szName = CPythonNetworkStream_GetAccountCharacterSlotDataz(pNetStream, uSlot, ACCOUNT_CHARACTER_SLOT_NAME);

			char szText[512];
			LIB_strnformat(szText, ARRAY_SIZE(szText), "(%s) : %s", szName, pMessage->szText);
			CPythonChat_AppendWhisper(pPythonChat, 0, pChat->szFrom, szText);
		}
		CPythonNetworkStream_SendWhisperPacket(pChat->pNetStream, pChat->szFrom, pMessage->szText);
	}
	CPythonNetworkStream_SendWhisperPacket(pChat->pNetStream, pChat->szFrom, "|?whisper_renewal<|");
	pMessage->iState = MESSAGE_OK;
}

void Chat_PollMessageSingleError(void *pPythonChat, Chat *pChat, ChatMessage *pMessage) {
	CPythonNetworkStream_SendWhisperPacket(pChat->pNetStream, pChat->szFrom, "|?whisper_renewal<|");
	pMessage->iState = MESSAGE_OK;
}

void Chat_PollMessageSingle(void *pPythonChat, Chat *pChat, ChatMessage *pMessage) {
	switch (pMessage->iState) {
		case MESSAGE_IN:
			Chat_PollMessageSingleInbound(pChat, pMessage);
			break;
		case MESSAGE_GPT:
			Chat_PollMessageSingleUploadGPT(pPythonChat, pChat, pMessage);
			Http_Upload(&pChat->mHistory, pMessage);
			break;
		case MESSAGE_GET:
			Chat_PollMessageSingleDownloadGPT(pPythonChat, pChat, pMessage);
			break;
		case MESSAGE_SND:
			Http_Complete(pMessage);
			Chat_PollMessageSingleSend(pPythonChat, pChat, pMessage);
			break;
		case MESSAGE_ERR:
			Http_Complete(pMessage);
			Chat_PollMessageSingleError(pPythonChat, pChat, pMessage);
			break;
	}
}

void Chat_PollMessage(void *pPythonChat) {
	LISTBASE_FOREACH_MUTABLE(Chat *, pChat, &Chats) {
		if (LIB_listbase_is_empty(&pChat->mHistory)) {
			LIB_remlink(&Chats, pChat);
			continue;
		}

		LISTBASE_FOREACH_MUTABLE(ChatMessage *, pMessage, &pChat->mHistory) {
			if (difftime(time(NULL), pMessage->tTime) > 60) {
				LIB_remlink(&pChat->mHistory, pMessage);
			}
		}

		ChatMessage *pMessage = (ChatMessage *)pChat->mHistory.last;
		if (!pMessage) {
			continue;
		}

		Chat_PollMessageSingle(pPythonChat, pChat, pMessage);
	}
}
