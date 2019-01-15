
#include "Hash.h"

#include <stdlib.h>

// Edited

namespace core
{

	//-----------------------------------------------------------------------------
	// MurmurHash3 was written by Austin Appleby, and is placed in the public
	// domain. The author hereby disclaims copyright to this source code.

	// Note - The x86 and x64 versions do _not_ produce the same results, as the
	// algorithms are optimized for their respective platforms. You can still
	// compile and run any of them on any platform, but your performance with the
	// non-native version will be less than optimal.

	u32 MurmurHash3_x86_32(const void* key, u32 len, u32 seed) {
		const u8* data = (const u8*) key;
		const i32 nblocks = (i32) len / 4;

		u32 hash = seed;

		const u32 c1 = 0xcc9e2d51;
		const u32 c2 = 0x1b873593;

		//----------
		// body
		const u32 * blocks = (const u32 *) (data + nblocks * 4);

		for (int i = -nblocks; i; i++) {
			u32 k1 = blocks[i];

			k1 *= c1;
			k1 = _rotl(k1, 15);
			k1 *= c2;

			hash ^= k1;
			hash = _rotl(hash, 13);
			hash = hash * 5 + 0xe6546b64;
		}

		//----------
		// tail

		const u8* tail = (const u8*) (data + nblocks * 4);

		u32 k1 = 0;

		switch (len & 3) {
		case 3: k1 ^= tail[2] << 16;
		case 2: k1 ^= tail[1] << 8;
		case 1: k1 ^= tail[0];
			k1 *= c1; k1 = _rotl(k1, 15); k1 *= c2; hash ^= k1;
		};

		//----------
		// finalization

		hash ^= len;

		hash ^= hash >> 16;
		hash *= 0x85ebca6b;
		hash ^= hash >> 13;
		hash *= 0xc2b2ae35;
		hash ^= hash >> 16;

		return hash;
	}



	//-----------------------------------------------------------------------------
	// MurmurHash2, 64-bit versions, by Austin Appleby

	// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
	// and endian-ness issues if used across multiple platforms.

	// 64-bit hash for 64-bit platforms

	u64 MurmurHash64A(const void * key, u32 len, u64 seed) {
		const uint64_t m = 0xc6a4a7935bd1e995llu;
		const int r = 47;

		uint64_t h = seed ^ (len * m);

		const uint64_t * data = (const uint64_t *) key;
		const uint64_t * end = data + (len / 8);

		while (data != end) {
			uint64_t k = *data++;

			k *= m;
			k ^= k >> r;
			k *= m;

			h ^= k;
			h *= m;
		}

		const unsigned char * data2 = (const unsigned char*) data;

		switch (len & 7) {
		case 7: h ^= uint64_t(data2[6]) << 48;
		case 6: h ^= uint64_t(data2[5]) << 40;
		case 5: h ^= uint64_t(data2[4]) << 32;
		case 4: h ^= uint64_t(data2[3]) << 24;
		case 3: h ^= uint64_t(data2[2]) << 16;
		case 2: h ^= uint64_t(data2[1]) << 8;
		case 1: h ^= uint64_t(data2[0]);
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return h;
	}
}