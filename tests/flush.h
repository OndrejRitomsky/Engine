#pragma once



#include <intrin.h>	

static void TestFlushCache(void* buffer, size_t size) {
	unsigned long long m = ((unsigned long long) buffer) % 128;
	char* b = (char*)(((unsigned long long) buffer) - m);

	size_t c = size / 128 + 1;
	for (size_t i = 0; i < c; ++i, b += 128) {
		_mm_clflush(b);
	}
}


static void TestFlushCache2(void** buffer, size_t count, size_t size) {
	for (size_t i = 0; i < count; ++i) {
		TestFlushCache(buffer[i], size);
	}
}