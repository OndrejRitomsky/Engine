#include "HandleMapBase.h"

#include "Core/Common/Pointer.h"
#include "Core/Common/Assert.h"
#include "Core/Algorithm/Memory.h"

namespace core {

	namespace handle_map_base {

		//-------------------------------------------------------------------------
		void Init(HandleMapBase* base, void* data, u32 capacity) {
			using namespace mem;
			base->freeHandleFirst = INTERNAL_INVALID_INDEX;
			base->freeHandleLast = INTERNAL_INVALID_INDEX;
			base->handleIndices = static_cast<u32*>(Align(data, alignof(u32)));
			base->handles = static_cast<HandleMapBase::InternalHandle*>(Align(&base->handleIndices[capacity], alignof(HandleMapBase::InternalHandle)));

			base->capacity = capacity;
			base->count = 0;
		}

		//---------------------------------------------------------------------------
		Handle Add(HandleMapBase* base, u32& outIndex) {
			HandleMapBase::InternalHandle handle;

			ASSERT(base->count < base->capacity);

			if (base->freeHandleFirst == INTERNAL_INVALID_INDEX) {
				HandleMapBase::InternalHandle innerHandle;

				innerHandle.index = base->count;
				innerHandle.generation = 0;
				innerHandle.type = base->handleType;
				innerHandle.free = 0;

				handle = innerHandle;
				handle.index = base->count;
				base->handles[base->count] = innerHandle;
			}
			else {
				u32 replacedHandleIndex = base->freeHandleFirst;

				HandleMapBase::InternalHandle& innerHandle = base->handles[replacedHandleIndex];

				ASSERT(innerHandle.free == 1);

				base->freeHandleFirst = innerHandle.index;

				if (base->freeHandleFirst == INTERNAL_INVALID_INDEX) 
					base->freeHandleLast = base->freeHandleFirst;

				ASSERT(base->count < base->capacity);
				innerHandle.index = base->count;
				innerHandle.generation++; // overflow
				innerHandle.free = 0;

				handle = innerHandle;
				handle.index = replacedHandleIndex;
			}

			base->handleIndices[base->count] = handle.index;
			outIndex = base->count;
			++base->count;
			return handle.value;
		}

		//---------------------------------------------------------------------------
		u32 Remove(HandleMapBase* base, const Handle& handle) {
			ASSERT(IsHandleValid(base, handle));
			--base->count;

			HandleMapBase::InternalHandle iHandle;
			iHandle.value = handle;

			HandleMapBase::InternalHandle& innerHandle = base->handles[iHandle.index];

			if (innerHandle.index < base->count) {
				// index wasnt last, update handles set
				u32 fixHandleIndex = base->handleIndices[base->count];

				base->handleIndices[innerHandle.index] = base->handleIndices[base->count];
				base->handles[fixHandleIndex].index = innerHandle.index;
			}

			innerHandle.index = INTERNAL_INVALID_INDEX;
			innerHandle.free = 1;

			if (base->freeHandleFirst == INTERNAL_INVALID_INDEX) {
				base->freeHandleFirst = iHandle.index;
				base->freeHandleLast = base->freeHandleFirst;
			}
			else {
				base->handles[base->freeHandleLast].index = iHandle.index;
				base->freeHandleLast = iHandle.index;
			}

			return iHandle.index;
		}

		//-------------------------------------------------------------------------
		u32 Get(const HandleMapBase* base, const Handle& handle) {
			ASSERT(IsHandleValid(base, handle));

			HandleMapBase::InternalHandle iHandle;
			iHandle.value = handle;

			return base->handles[iHandle.index].index;
		}

		//-------------------------------------------------------------------------
		bool IsHandleValid(const HandleMapBase* base, const Handle& handle) {
			HandleMapBase::InternalHandle iHandle;
			iHandle.value = handle;

			if (iHandle.value == INVALID_HANDLE || iHandle.free == 1 || iHandle.type != base->handleType || iHandle.index >= base->capacity)
				return false;

			HandleMapBase::InternalHandle innerHandle = base->handles[iHandle.index];
			return innerHandle.free == 0 && innerHandle.generation == iHandle.generation && innerHandle.index < base->count;
		}

		//-------------------------------------------------------------------------
		u32 SizeRequiredForCapacity(u32 capacity) {
			return capacity * (sizeof(u32) + sizeof(h64)) + alignof(u32) + alignof(h64);
		}

		//-------------------------------------------------------------------------
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