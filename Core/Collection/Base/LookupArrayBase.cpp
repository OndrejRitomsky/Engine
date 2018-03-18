#include "LookupArrayBase.h"

#include "Core/CoreAssert.h"

namespace core {

	static const u32 INVALID_INDEX = U32MAX;

	namespace lookup_array_base {
		//-------------------------------------------------------------------------
		void Clear(LookupArrayBase* base) {
			base->freeFirst = INVALID_INDEX;
			base->size = 0;
		}

		//-------------------------------------------------------------------------
		void Remove(LookupArrayBase* base, u32 index) {
			ASSERT(index < base->size);

			u32* element = reinterpret_cast<u32*>(static_cast<char*>(base->data) + index * base->elementSize);
			*element = base->freeFirst;
			base->freeFirst = index;
		}

		//-------------------------------------------------------------------------
		void* Get(const LookupArrayBase* base, u32 index) {
			ASSERT(base->data);
			ASSERT(index < base->size);
			return static_cast<char*>(base->data) + index * base->elementSize;
		}

		//-------------------------------------------------------------------------
		u32 Add(LookupArrayBase* base) {
			if (base->freeFirst == INVALID_INDEX) {
				ASSERT(!IsFull(base));
				++base->size;
				return base->size - 1;
			}
			else {
				u32 result = base->freeFirst * base->elementSize;
				u32* element = reinterpret_cast<u32*>(static_cast<char*>(base->data) + result);
				base->freeFirst = *element;
				return result;
			}
		}

		//-------------------------------------------------------------------------
		bool IsFull(const LookupArrayBase* base) {
			return base->freeFirst == INVALID_INDEX && base->size == base->capacity;
		}
	}
}
