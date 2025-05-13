#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void LogSpawn();
bool LogPoll(char *szText, size_t dwLength);

void LogConnect();
void LogDisconnect();

void Log(const char *msg);
void Logf(ATTR_PRINTF_FORMAT const char *msg, ...);

#ifdef __cplusplus
}
#endif

#endif
