#pragma once

#include "Core/Allocator/IAllocator.h"

#include "Core/Utility.h"
#include "Core/CoreAssert.h"

#include "Core/Memory/Pointer.h"

namespace core {
	//-------------------------------------------------------------------------
	static u64 ArrayHeaderSizeForType(u32 headerSize, u64 typeAlignment) {
		ASSERT(mem::IsAlignmentPowerOfTwo(typeAlignment));
		u64 alignedHeader = typeAlignment * (headerSize / typeAlignment);
		if (headerSize % typeAlignment > 0)
			alignedHeader += typeAlignment;
		return alignedHeader;
	}


	//-----------------------------------------------------------------------------
	template<typename T, typename... Args>
	inline T* IAllocator::Make(Args&&... args) {
		void* address = Allocate(sizeof(T), alignof(T));
		if (!address)
			return nullptr;

		return new (address) T(forward<Args>(args)...);
	}

	//-----------------------------------------------------------------------------
	template<typename T>
	inline void IAllocator::Destroy(T* object) {
		ASSERT(object != nullptr);
		object->~T();
		Deallocate(object);
	}

	//-----------------------------------------------------------------------------
	template<typename T, typename... Args>
	inline T* IAllocator::MakeArray(u32 count, Args&&... args) {
		u64 headerSize = ArrayHeaderSizeForType(sizeof(u32), alignof(T));

		u64 size = (u64) count * sizeof(T);

		void* address = Allocate(size + headerSize, alignof(T));
		if (!address)
			return nullptr;

		T* p = (T*) mem::Add(address, headerSize);

		*(((u32*) p) - 1) = count;
		for (u32 i = 0; i < count; ++i) {
			new (&p[i]) T(forward<Args>(args)...);
		}

		return p;
	}

	//-----------------------------------------------------------------------------
	template<typename T>
	inline void IAllocator::DestroyArray(T* objects) {
		ASSERT(objects != nullptr);
		u64 headerSize = ArrayHeaderSizeForType(sizeof(u32), alignof(T));

		u32 count = *(((u32*) objects) - 1);
		for (u32 i = 0; i < count; ++i) {
			objects[i].~T();
		}

		void* address = mem::Sub(objects, headerSize);

		Deallocate(address);
	}

}