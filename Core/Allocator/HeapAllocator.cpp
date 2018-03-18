#include "HeapAllocator.h"

#include "Core/CoreAssert.h"
#include "Core/Memory/Pointer.h"

#include <cstdio>

#include <malloc.h>

namespace core {
	//---------------------------------------------------------------------------
	HeapAllocator::HeapAllocator() :
		_allocationsCount(0) {
	}
	
	//---------------------------------------------------------------------------
	HeapAllocator::~HeapAllocator() {
		ASSERT(_allocationsCount == 0);
	}

	//---------------------------------------------------------------------------
	void* HeapAllocator::Allocate(u64 size, u64 allignment) {
		ASSERT(mem::IsAlignmentPowerOfTwo(allignment));
		_allocationsCount++;

		return _aligned_malloc(size, allignment);
	}

	//---------------------------------------------------------------------------
	void HeapAllocator::Deallocate(void* address) {
		if (address) {
			_aligned_free(address);
			_allocationsCount--;
		}
	}
}