// Copyright (c) Kuba Szczodrzyński 2024-12-7.

#pragma once

#include "include.h"

#define MALLOC(ptr, size, err)                                                                                         \
	do {                                                                                                               \
		ptr = malloc(size);                                                                                            \
		if (ptr == NULL) {                                                                                             \
			LT_E("Memory allocation failed for '" #ptr "' (%llu bytes)", (unsigned long long)size);                    \
			err;                                                                                                       \
		}                                                                                                              \
		memset(ptr, 0, size);                                                                                          \
	} while (0)

#define LT_ERR(level, err, ...)                                                                                        \
	do {                                                                                                               \
		LT_##level(__VA_ARGS__);                                                                                       \
		err;                                                                                                           \
	} while (0)

#define SOCK_ERROR(func, err)                                                                                          \
	do {                                                                                                               \
		LT_F(func " failed; errno: %s", strerror(errno));                                                              \
		err;                                                                                                           \
	} while (0)

#define SQLITE3_ERROR(func, err)                                                                                       \
	do {                                                                                                               \
		LT_F(func " failed; errmsg: %s", sqlite3_errmsg(db));                                                          \
		err;                                                                                                           \
	} while (0)
