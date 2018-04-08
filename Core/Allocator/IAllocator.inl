#pragma once

#include "Core/Allocator/IAllocator.h"

#include "Core/Common/Assert.h"
#include "Core/Common/Placement.h"

namespace core {
	//---------------------------------------------------------------------------
	inline void* IAllocator::Allocate(u64 size, u64 alignment, u64* outAllocated) {
		return (this->*_Allocate)(size, alignment, outAllocated);
	}

	//---------------------------------------------------------------------------
	inline void IAllocator::Deallocate(void* address) {
		return (this->*_Deallocate)(address);
	}

	//---------------------------------------------------------------------------
	inline void IAllocator::Deinit() {
		return (this->*_Deinit)();
	}

	//---------------------------------------------------------------------------
	template<typename T, typename... Args>
	inline T* IAllocator::Make(Args&&... args) {
		void* address = Allocate(sizeof(T), alignof(T));
		if (!address)
			return nullptr;

		return Placement(address) T(forward<Args>(args)...);
	}

	//---------------------------------------------------------------------------
	template<typename T>
	inline void IAllocator::Destroy(T* object) {
		ASSERT(object != nullptr);
		object->~T();
		Deallocate(object);
	}


}