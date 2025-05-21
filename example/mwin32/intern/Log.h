#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void LogSpawn();
bool LogPoll(char *szText, size_t dwLength);

void Log(const char *msg);
void Logf(ATTR_PRINTF_FORMAT const char *msg, ...);

#define ALWAYS_ASSERT(a) (void)((!(a)) ? ((Logf("Assertion failed: %s:%d, %s(), at \'%s\'", __FILE__, __LINE__, __FUNCTION__, #a), abort(), NULL)) : NULL)

#ifdef __cplusplus
}
#endif

#endif
