#pragma once

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Edited

#include "../common/types.h"


namespace core
{
	u32 MurmurHash3_x86_32(const void * key, u32 len, u32 seed);

	u64 MurmurHash64A(const void * key, u32 len, u64 seed);

	//u32 ToHash32(const void* key, u32 length);



	/*inline Hash32 ToHash32(const void* key, u32 length) {
		static_assert(sizeof(Hash32) == sizeof(u32), "Hash size must match");
		return (Hash32) MurmurHash3_x86_32(key, length, 314953lu);
	}*/


	inline h64 Hash(const void* key, u32 length) {
		static_assert(sizeof(h64) == sizeof(u64), "Hash size must match");
		return (h64) MurmurHash64A(key, length, 314953llu);
	}


}