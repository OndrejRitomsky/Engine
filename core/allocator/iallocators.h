#pragma once

#include "../common/types.h"

namespace core {
	struct IAllocator;

	typedef void*(*IAllocatorAllocateFn)(IAllocator* allocator, u64 size, u64 alignment, u64* outAllocated);
	typedef void(*IAllocatorDeallocateFn)(IAllocator* allocator, void* mem);
	typedef void(*IAllocatorDeinitFn)(IAllocator* allocator);

	struct IAllocator {
		IAllocatorAllocateFn Allocate;
		IAllocatorDeallocateFn Deallocate;
		IAllocatorDeinitFn Deinit;
	};


	struct ITagAllocator;

	typedef void*(*ITagAllocatorAllocateFn)(ITagAllocator* allocator, const MemTag& tag, u64 size, u64 alignment, u64* outAllocated);
	typedef void(*ITagAllocatorDeallocateFn)(ITagAllocator* allocator, const MemTag& tag);
	typedef void(*ITagAllocatorDeinitFn)(ITagAllocator* allocator);

	struct ITagAllocator {
		ITagAllocatorAllocateFn Allocate;
		ITagAllocatorDeallocateFn Deallocate;
		ITagAllocatorDeinitFn Deinit;
	};
	
}


