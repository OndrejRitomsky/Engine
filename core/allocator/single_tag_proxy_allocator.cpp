#include "../allocator/allocators.h"

#include "allocate.h"
#include "../common/debug.h"


namespace core {
	static void* SingleTagProxyAllocatorAllocate(IAllocator* allocator, u64 size, u64 alignment, u64* outAllocated) {
		SingleTagProxyAllocator* all = (SingleTagProxyAllocator*) allocator;
		return TagAllocate(all->_backingAllocator, all->_tag, size, alignment, outAllocated);
	}

	static void SingleTagProxyAllocatorDeallocate(IAllocator* allocator, void* mem) {
		ASSERT(false);
	}

	static void SingleTagProxyAllocatorDeinit(IAllocator* allocator) {
	}

	IAllocator* SingleTagProxyAllocatorInit(SingleTagProxyAllocator* allocator, ITagAllocator* backingAllocator, MemTag tag) {
		allocator->_tag = tag;
		allocator->_backingAllocator = backingAllocator;
		allocator->allocator.Allocate = SingleTagProxyAllocatorAllocate;
		allocator->allocator.Deallocate = SingleTagProxyAllocatorDeallocate;
		allocator->allocator.Deinit = SingleTagProxyAllocatorDeinit;
		return &allocator->allocator;
	}
}