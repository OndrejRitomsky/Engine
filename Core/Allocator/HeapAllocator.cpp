#include "HeapAllocator.h"

#include "Core/Common/Assert.h"
#include "Core/Common/Pointer.h"

#include <cstdio>

#include <malloc.h>

namespace core {
	//---------------------------------------------------------------------------
	HeapAllocator::HeapAllocator() :
		_allocationsCount(0) {

		_Allocate = static_cast<IAllocator::AllocateFunction>(&HeapAllocator::Allocate);
		_Deallocate = static_cast<IAllocator::DeallocateFunction>(&HeapAllocator::Deallocate);
		_Deinit = static_cast<IAllocator::DeinitFunction>(&HeapAllocator::Deinit);
	}
	
	//---------------------------------------------------------------------------
	HeapAllocator::~HeapAllocator() {
		Deinit();
	}

	//---------------------------------------------------------------------------
	void* HeapAllocator::Allocate(u64 size, u64 alignment, u64* outAllocated) {
		ASSERT(mem::IsAlignmentPowerOfTwo(alignment));
		_allocationsCount++;

		if (outAllocated) {
			*outAllocated = size;
		}

		return _aligned_malloc(size, alignment);
	}

	//---------------------------------------------------------------------------
	void HeapAllocator::Deallocate(void* address) {
		if (address) {
			_aligned_free(address);
			_allocationsCount--;
		}
	}

	void HeapAllocator::Deinit() {
		ASSERT(_allocationsCount == 0);
	}
}