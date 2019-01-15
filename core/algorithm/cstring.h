#pragma once

#include "../common/types.h"

namespace core {
	u32 Format(char* buffer, u32 bufferSize, const char* format, ...);

	void Memcpy(void* dest, const void* source, u64 size);

	void Memset(void* data, i32 value, u64 size);

	u32 Strlen(const char* data);

	bool StrEqual(const char* str1, const char* str2);
}