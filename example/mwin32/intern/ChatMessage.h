#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <time.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ChatMessage {
	struct ChatMessage *prev, *next;

	char szName[64];
	char szText[512];

	int iState;

	time_t tTime;

	HANDLE hSession;
	HANDLE hConnect;
	HANDLE hRequest;
} ChatMessage;

enum {
	MESSAGE_IN,
	MESSAGE_GPT,
	MESSAGE_GET,
	MESSAGE_SND,
	MESSAGE_ERR,
	MESSAGE_OK,
};

#ifdef __cplusplus
}
#endif

#endif	// CHAT_MESSAGE_H