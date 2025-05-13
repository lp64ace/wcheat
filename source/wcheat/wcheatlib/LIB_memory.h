#ifndef LIB_MEMORY_H
#define LIB_MEMORY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t LIB_pattern_offset_ex(const void *base, size_t size, const unsigned char *bytes, const unsigned char *mask, size_t length);
uintptr_t LIB_pattern_offset(const void *base, size_t size, const char *text);

#ifdef __cplusplus
}
#endif

#endif // LIB_MEMORY_H
