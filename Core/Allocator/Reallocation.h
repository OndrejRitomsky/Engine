#pragma once

#include "Core/Types.h"
#include "Core/Utility.h"
#include "Core/Allocator/IAllocator.h"

namespace core {


	void* ReallocateMemCopy(IAllocator* allocator, void* data, u64 size, u64 alignment, u64 newCapacity);


	template<typename Type>
	Type* ReallocateMove(IAllocator* allocator, Type* data, u64 count, u64 newCapacity);


	//-------------------------------------------------------------------------
	inline void* ReallocateMemCopy(IAllocator* allocator, void* data, u64 size, u64 alignment, u64 newCapacity) {
		void* result = allocator->Allocate(newCapacity, alignment);

		if (result && data) {
			memcpy(result, data, size);
			allocator->Deallocate(data);
		}

		return result;
	}

	//-------------------------------------------------------------------------
	template<typename Type>
	inline Type* ReallocateMove(IAllocator* allocator, Type* data, u64 count, u64 newCapacity) {
		Type* result = (Type*) allocator->Allocate(newCapacity * sizeof(Type), alignof(Type));

		if (result) {
			for (u32 i = 0; i < count; ++i) {
				result[i] = move(data[i]);
				data[i].~Type();
			}
		}

		if (data)
			allocator->Deallocate(data);

		return result;
	}

}