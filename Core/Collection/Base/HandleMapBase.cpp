#include "handlemapbase.h"

#include "../../common/pointer.h"
#include "../../common/debug.h"
#include "../../algorithm/cstring.h"

namespace core {

	namespace handle_map_base {
		void Init(HandleMapBase* base, void* data, u32 capacity) {
			base->freeHandleFirst = INTERNAL_INVALID_INDEX;
			base->freeHandleLast = INTERNAL_INVALID_INDEX;
			base->handleIndices = static_cast<u32*>(PointerAlign(data, alignof(u32)));
			base->handles = static_cast<HandleMapBase::InternalHandle*>(PointerAlign(&base->handleIndices[capacity], alignof(HandleMapBase::InternalHandle)));

			base->capacity = capacity;
			base->count = 0;
		}

		Handle Add(HandleMapBase* base, u32& outIndex) {
			HandleMapBase::InternalHandle handle;

			ASSERT(base->count < base->capacity);

			if (base->freeHandleFirst == INTERNAL_INVALID_INDEX) {
				HandleMapBase::InternalHandle innerHandle;

				innerHandle.internal.index = base->count;
				innerHandle.internal.generation = 0;
				innerHandle.internal.type = base->handleType;
				innerHandle.internal.free = 0;

				handle = innerHandle;
				handle.internal.index = base->count;
				base->handles[base->count] = innerHandle;
			}
			else {
				u32 replacedHandleIndex = base->freeHandleFirst;

				HandleMapBase::InternalHandle& innerHandle = base->handles[replacedHandleIndex];

				ASSERT(innerHandle.internal.free == 1);

				base->freeHandleFirst = innerHandle.internal.index;

				if (base->freeHandleFirst == INTERNAL_INVALID_INDEX) 
					base->freeHandleLast = base->freeHandleFirst;

				ASSERT(base->count < base->capacity);
				innerHandle.internal.index = base->count;
				innerHandle.internal.generation++; // overflow
				innerHandle.internal.free = 0;

				handle = innerHandle;
				handle.internal.index = replacedHandleIndex;
			}

			base->handleIndices[base->count] = handle.internal.index;
			outIndex = base->count;
			++base->count;
			return handle.value;
		}

		u32 Remove(HandleMapBase* base, const Handle& handle) {
			ASSERT(IsHandleValid(base, handle));
			--base->count;

			HandleMapBase::InternalHandle iHandle;
			iHandle.value = handle;

			HandleMapBase::InternalHandle& innerHandle = base->handles[iHandle.internal.index];

			if (innerHandle.internal.index < base->count) {
				// index wasnt last, update handles set
				u32 fixHandleIndex = base->handleIndices[base->count];

				base->handleIndices[innerHandle.internal.index] = base->handleIndices[base->count];
				base->handles[fixHandleIndex].internal.index = innerHandle.internal.index;
			}

			innerHandle.internal.index = INTERNAL_INVALID_INDEX;
			innerHandle.internal.free = 1;

			if (base->freeHandleFirst == INTERNAL_INVALID_INDEX) {
				base->freeHandleFirst = iHandle.internal.index;
				base->freeHandleLast = base->freeHandleFirst;
			}
			else {
				base->handles[base->freeHandleLast].internal.index = iHandle.internal.index;
				base->freeHandleLast = iHandle.internal.index;
			}

			return iHandle.internal.index;
		}

		u32 Get(const HandleMapBase* base, const Handle& handle) {
			ASSERT(IsHandleValid(base, handle));

			HandleMapBase::InternalHandle iHandle;
			iHandle.value = handle;

			return base->handles[iHandle.internal.index].internal.index;
		}

		bool IsHandleValid(const HandleMapBase* base, const Handle& handle) {
			HandleMapBase::InternalHandle iHandle;
			iHandle.value = handle;

			if (iHandle.value == INVALID_HANDLE || iHandle.internal.free == 1 || iHandle.internal.type != base->handleType || iHandle.internal.index >= base->capacity)
				return false;

			HandleMapBase::InternalHandle innerHandle = base->handles[iHandle.internal.index];
			return innerHandle.internal.free == 0 && innerHandle.internal.generation == iHandle.internal.generation && innerHandle.internal.index < base->count;
		}

		u32 SizeRequiredForCapacity(u32 capacity) {
			return capacity * (sizeof(u32) + sizeof(h64)) + alignof(u32) + alignof(h64);
		}

		void Copy(HandleMapBase* dest, HandleMapBase* source) {
			if (dest->handles != source->handles) {
				Memcpy(dest->handleIndices, source->handleIndices, source->count * sizeof(u32));
				Memcpy(dest->handles, source->handles, source->capacity * sizeof(HandleMapBase::InternalHandle));
			}

			dest->count = source->count;
			dest->capacity = source->capacity;
			dest->handleType = source->handleType;
		}
	}
}