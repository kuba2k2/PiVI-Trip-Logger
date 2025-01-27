// Copyright (c) Kuba Szczodrzyński 2024-12-16.

#include "utils.h"

void hexdump(const void *buf, size_t len) {
	size_t pos = 0;
	while (pos < len) {
		// print hex offset
		printf("%06x ", (unsigned int)pos);
		// calculate current line width
		size_t lineWidth = min(16, len - pos);
		// print hexadecimal representation
		for (size_t i = 0; i < lineWidth; i++) {
			if (i % 8 == 0) {
				putchar(' ');
			}
			printf("%02x ", ((const uint8_t *)buf)[pos + i]);
		}
		// print ascii representation
		putchar(' ');
		putchar('|');
		for (size_t i = 0; i < lineWidth; i++) {
			uint8_t c = ((const uint8_t *)buf)[pos + i];
			putchar((c >= 0x20 && c <= 0x7f) ? c : '.');
		}
		puts("|\r");
		pos += lineWidth;
	}
	fflush(stdout);
}

unsigned long long millis() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

char *strncpy2(char *dest, const char *src, size_t count) {
	strncpy(dest, src, count);
	dest[count] = '\0';
	return dest;
}
