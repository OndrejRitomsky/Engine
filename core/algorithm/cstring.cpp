#include "cstring.h"

#include <string.h>

#include "format.h"

namespace core {
	u32 Format(char* buffer, u32 bufferSize, const char* format, ...) {
		va_list args;
		va_start(args, format);

		unsigned int needed = FormatVA(buffer, bufferSize, format, args);

		va_end(args);

		return needed;
	}

	void Memcpy(void* dest, const void* source, u64 size) {
		memcpy(dest, source, size);
	}

	void Memset(void* data, i32 value, u64 size) {
		memset(data, value, size);
	}

	u32 Strlen(const char* data) {
		return (u32) strlen(data);
	}
}