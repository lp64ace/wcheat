#ifndef CHAT_H
#define CHAT_H

#ifdef __cplusplus
extern "C" {
#endif

void Chat_PostMessage(void *pNetworkStream, const char *szNameFrom, const char *szChat);
void Chat_PollMessage(void *pChat);

#ifdef __cplusplus
}
#endif

#endif // CHAT_H
