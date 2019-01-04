#pragma once

#include "../common/types.h"

namespace core {
	struct IAllocator;
	struct ITagAllocator;


	void* Allocate(IAllocator* allocator, u64 size, u64 alignment);

	void* Allocate(IAllocator* allocator, u64 size, u64 alignment, u64* outAllocated);

	void Deallocate(IAllocator* allocator, void* mem);

	void AllocatorDeinit(IAllocator* allocator);


	void* TagAllocate(ITagAllocator* allocator, MemTag tag, u64 size, u64 alignment);

	void* TagAllocate(ITagAllocator* allocator, MemTag tag, u64 size, u64 alignment, u64* outAllocated);

	void TagDeallocate(ITagAllocator* allocator, MemTag tag);

	void TagAllocatorDeinit(ITagAllocator* allocator);
}