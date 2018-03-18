#pragma once


#include "Core/Types.h"
#include "Core/CoreAssert.h"


namespace core {
	namespace mem {
		void* Add(void* address, u64 add);
		void* Sub(void* address, u64 sub);

		const void* Add(const void* address, u64 add);
		const void* Sub(const void* address, u64 sub);

		// Result is signed, dont measure pointers which arent from the same allocation + 1
		u64 SizeBetween(void* higherAddress, void* lowerAddress);

		bool IsAlignmentPowerOfTwo(u64 x);

		template<typename T>
		T* Align(void* address);

		void* Align(void* address, u64 alignment);

		u64 Adjustment(void* address, u64 alignment);

		u64 AdjustmentWithHeader(void* address, u64 alignment, u64 headerSize);

		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------


		//-------------------------------------------------------------------------
		inline void* Add(void* address, u64 add) {
			return (void*) (((char*) address) + add);
		}

		//-------------------------------------------------------------------------
		inline void* Sub(void* address, u64 sub) {
			return (void*) (((char*) address) - sub);
		}

		//-------------------------------------------------------------------------
		inline const void* Add(const void* address, u64 add) {
			return (void*) (((char*) address) + add);
		}

		//-------------------------------------------------------------------------
		inline const void* Sub(const void* address, u64 sub) {
			return (void*) (((char*) address) - sub);
		}

		//-------------------------------------------------------------------------
		inline u64 SizeBetween(void* higherAddress, void* lowerAddress) {
			ASSERT((u64) higherAddress > (u64) lowerAddress);
			return ((u64) higherAddress - (u64) lowerAddress);
		}

		//-------------------------------------------------------------------------
		inline bool IsAlignmentPowerOfTwo(u64 x) {
			return x != 0 && !(x & (x - 1));
		}


		template<typename T>
		inline T* Align(void* address) {
			return (T*) Align(address, alignof(T));
		}

		//-------------------------------------------------------------------------
		inline void* Align(void* address, u64 alignment) {
			ASSERT(IsAlignmentPowerOfTwo(alignment));
			return (void*) ((((u64) address) + alignment - 1) & (~(alignment - 1)));
		}

	

		//-------------------------------------------------------------------------
		inline u64 Adjustment(void* address, u64 alignment) {
			ASSERT(IsAlignmentPowerOfTwo(alignment));
			u64 adjustment = alignment - (((u64) address) & (alignment - 1));
			return adjustment % alignment;
		}

		//-------------------------------------------------------------------------
		inline size_t AdjustmentWithHeader(void* address, u64 alignment, u64 headerSize) {
			ASSERT(IsAlignmentPowerOfTwo(alignment));

			u64 adjustment = Adjustment(address, alignment);
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
}