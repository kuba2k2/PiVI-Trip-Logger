// Copyright (c) Kuba Szczodrzyński 2024-12-16.

#pragma once

#include <stdlib.h>

#define STRUCT_PADDING(field, len) char _padding_##field[4 - (len) % 4]
#define BUILD_BUG_ON(condition)	   ((void)sizeof(char[1 - 2 * !!(condition)]))

#define CONCAT_(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix)	CONCAT_(prefix, suffix)
#define UNIQ(name)				CONCAT(name, __LINE__)

#define FREE_NULL(var)                                                                                                 \
	do {                                                                                                               \
		free(var);                                                                                                     \
		var = NULL;                                                                                                    \
	} while (0)

#ifndef min
#define min(a, b)                                                                                                      \
	({                                                                                                                 \
		__typeof__(a) _a = (a);                                                                                        \
		__typeof__(b) _b = (b);                                                                                        \
		_a < _b ? _a : _b;                                                                                             \
	})
#endif

#ifndef max
#define max(a, b)                                                                                                      \
	({                                                                                                                 \
		__typeof__(a) _a = (a);                                                                                        \
		__typeof__(b) _b = (b);                                                                                        \
		_a > _b ? _a : _b;                                                                                             \
	})
#endif

#include "include.h"

void hexdump(const void *buf, size_t len);
unsigned long long millis();
char *strncpy2(char *dest, const char *src, size_t count);
