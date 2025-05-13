#ifndef LIB_HEX_H
#define LIB_HEX_H

#include "LIB_utildefines.h"

#ifdef __cplusplus
extern "C" {
#endif

WCHEAT_INLINE unsigned char LIB_hex2half(const char hex[1]);
WCHEAT_INLINE unsigned char LIB_hex2byte(const char hex[2]);

#ifdef __cplusplus
}
#endif

#include "intern/hex_inline.c"

#endif
