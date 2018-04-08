#include "Memory.h"

#include <memory>

//-----------------------------------------------------------------------------
void Memcpy(void* dest, const void* source, u64 size) {
	memcpy(dest, source, size);
}

//-----------------------------------------------------------------------------
void Memset(void* data, i32 value, u64 size) {
	memset(data, value, size);
}