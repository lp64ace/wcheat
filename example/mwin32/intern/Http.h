#ifndef HTTP_H
#define HTTP_H

#include "LIB_listbase.h"

#include "Chat.h"
#include "ChatMessage.h"

#ifdef __cplusplus
extern "C" {
#endif

void Http_Upload(ListBase *pHistory, ChatMessage *pMessage);
void Http_Poll(ChatMessage *pMessage);
void Http_Complete(ChatMessage *pMessage);

#ifdef __cplusplus
}
#endif

#endif