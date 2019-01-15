#pragma once

#include "types.h"
#include "debug.h"


// @TODO remove mem namespace and add prefix Pointer

namespace core {
	void* PointerAdd(void* address, u64 add);
	void* PointerSub(void* address, u64 sub);

	const void* PointerAdd(const void* address, u64 add);
	const void* PointerSub(const void* address, u64 sub);

	// Result is signed, dont measure pointers which arent from the same allocation + 1
	u64 PointerSizeBetween(void* higherAddress, void* lowerAddress);

	bool IsAlignmentPowerOfTwo(u64 x);

	/*template<typename T>
	T* PointerAlign(void* address);*/

	void* PointerAlign(void* address, u64 alignment);

	u64 PointerAdjustment(void* address, u64 alignment);

	u64 PointerAdjustmentWithHeader(void* address, u64 alignment, u64 headerSize);


	inline void* PointerAdd(void* address, u64 add) {
		return (void*) (((char*) address) + add);
	}

	inline void* PointerSub(void* address, u64 sub) {
		return (void*) (((char*) address) - sub);
	}

	inline const void* PointerAdd(const void* address, u64 add) {
		return (void*) (((char*) address) + add);
	}

	inline const void* PointerSub(const void* address, u64 sub) {
		return (void*) (((char*) address) - sub);
	}

	inline u64 PointerSizeBetween(void* higherAddress, void* lowerAddress) {
		ASSERT((u64) higherAddress > (u64) lowerAddress);
		return ((u64) higherAddress - (u64) lowerAddress);
	}

	inline bool IsAlignmentPowerOfTwo(u64 x) {
		return x != 0 && !(x & (x - 1));		
	}

	/*template<typename T>
	inline T* PointerAlign(void* address) {
		return (T*) PointerAlign(address, alignof(T));
	}*/

	inline void* PointerAlign(void* address, u64 alignment) {
		ASSERT(IsAlignmentPowerOfTwo(alignment));
		return (void*) ((((u64) address) + alignment - 1) & (~(alignment - 1)));
	}

	inline u64 PointerAdjustment(void* address, u64 alignment) {
		ASSERT(IsAlignmentPowerOfTwo(alignment));
		u64 adjustment = alignment - (((u64) address) & (alignment - 1));
		return adjustment % alignment;
	}

	inline size_t PointerAdjustmentWithHeader(void* address, u64 alignment, u64 headerSize) {
		ASSERT(IsAlignmentPowerOfTwo(alignment));

		u64 adjustment = PointerAdjustment(address, alignment);
		u64 spaceNeeded = headerSize;

		if (adjustment < spaceNeeded) {
			spaceNeeded -= adjustment;

			adjustment = alignment * (spaceNeeded / alignment);

			if (spaceNeeded % alignment > 0)
				adjustment += alignment;
		}
		return adjustment;
	}
}