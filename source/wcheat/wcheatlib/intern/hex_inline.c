#include "LIB_hex.h"

#ifdef __cplusplus
extern "C" {
#endif

WCHEAT_INLINE unsigned char LIB_hex2half(const char hex[1]) {
	WCHEAT_assert(IN_RANGE_INCL(hex[0], '0', '9') || IN_RANGE_INCL(hex[0], 'a', 'f') || IN_RANGE_INCL(hex[0], 'A', 'F'));
	if ('0' <= hex[0] && hex[0] <= '9') return hex[0] - '0';
	if ('A' <= hex[0] && hex[0] <= 'F') return hex[0] - 'A' + 10;
	if ('a' <= hex[0] && hex[0] <= 'f') return hex[0] - 'a' + 10;
	return 0xf;
}
WCHEAT_INLINE unsigned char LIB_hex2byte(const char hex[2]) {
	return (LIB_hex2half(hex + 0) << 4) | LIB_hex2half(hex + 1);
}

#ifdef __cplusplus
}
#endif
