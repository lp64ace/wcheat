#include "MEM_guardedalloc.h"

#include "LIB_hex.h"
#include "LIB_memory.h"
#include "LIB_string.h"

uintptr_t LIB_pattern_offset_ex(const void *base, size_t size, const unsigned char *bytes, const unsigned char *mask, size_t length) {
	for (const void *itr = base; itr != POINTER_OFFSET(base, size - (length)); itr = POINTER_OFFSET(itr, 1)) {
		size_t index = 0;
		while (index < length) {
			unsigned char byte = (mask) ? mask[index] : 0xff;
			if (((*(const unsigned char *)POINTER_OFFSET(itr, index)) & byte) != (bytes[index] & byte)) {
				break;
			}
			if (++index == length) {
				const size_t remaining = (const char *)POINTER_OFFSET(base, size) - (const char *)itr;
				/** Do not allow patterns that match different parts of the program! */
				WCHEAT_assert(LIB_pattern_offset_ex(POINTER_OFFSET(itr, 1), remaining, bytes, mask, length) == 0);
				return (uintptr_t)itr;
			}
		}
	}
	return (uintptr_t)NULL;
}

uintptr_t LIB_pattern_offset(const void *base, size_t size, const char *text) {
	unsigned char *bytes = MEM_mallocN(LIB_strlen(text), "pattern::bytes");
	unsigned char *masks = MEM_mallocN(LIB_strlen(text), "pattern::masks");
	size_t length = 0;

	for (const char *itr = text; !ELEM(itr[0], '\0'); itr++) {
		if (ELEM(itr[0], ' ', '\t', '\n', '\r')) {
			continue;
		}
		
		if (ELEM(itr[0], '?') && ELEM(itr[1], '?')) {
			bytes[length] = 0x00;
			masks[length++] = 0x00;
			itr++;
		}
		else {
			bytes[length] = LIB_hex2byte(itr);
			masks[length++] = 0xff;
			itr++;
		}
	}

	uintptr_t offset = LIB_pattern_offset_ex(base, size, bytes, masks, length);
	MEM_freeN(bytes);
	MEM_freeN(masks);
	return offset;
}
