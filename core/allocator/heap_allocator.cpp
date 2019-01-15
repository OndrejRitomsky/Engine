#include "allocators.h"

#include "../common/debug.h"
#include "../common/pointer.h"

#include <malloc.h>

namespace core {
	static void* HeapAllocate(IAllocator* allocator, u64 size, u64 alignment, u64* outAllocated) {
		ASSERT(IsAlignmentPowerOfTwo(alignment));

		HeapAllocator* heapAll = (HeapAllocator*) allocator;
		heapAll->_allocationsCount++;

		if (outAllocated) {
			*outAllocated = size;
		}

		return _aligned_malloc(size, alignment);
	}

	static void HeapDeallocate(IAllocator* allocator, void* mem) {
		HeapAllocator* heapAll = (HeapAllocator*) allocator;
		if (mem) {
			_aligned_free(mem);
			heapAll->_allocationsCount--;
		}
	}

	static void HeapDeinit(IAllocator* allocator) {
		HeapAllocator* heapAll = (HeapAllocator*) allocator;
		ASSERT(heapAll->_allocationsCount == 0);
	}

	IAllocator* HeapAllocatorInit(HeapAllocator* heapAllocator) {
		heapAllocator->allocator.Allocate = HeapAllocate;
		heapAllocator->allocator.Deallocate = HeapDeallocate;
		heapAllocator->allocator.Deinit = HeapDeinit;
		heapAllocator->_allocationsCount = 0;
		return &heapAllocator->allocator;
	}
}