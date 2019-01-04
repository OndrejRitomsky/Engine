#pragma once

#include <stdint.h>

typedef uintptr_t uptr;

typedef ptrdiff_t ptrdiff;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef uint32_t h32;
typedef uint64_t h64;

namespace core {
	typedef u64 Handle;
	typedef u64 MemTag;
}

static const u32 U32MIN = 0u;
static const u32 U32MAX = 0xFFFFFFFFu;

static const u64 U64MIN = 0u;
static const u64 U64MAX = 0xFFFFFFFFFFFFFFFFu;

static const u64 INVALID_HANDLE = U64MAX;

