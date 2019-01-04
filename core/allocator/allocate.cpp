#include "allocate.h"

#include "allocators.h"

namespace core {
	void* Allocate(IAllocator* allocator, u64 size, u64 alignment) {
		return allocator->Allocate(allocator, size, alignment, nullptr);
	}

	void* Allocate(IAllocator* allocator, u64 size, u64 alignment, u64* outAllocated) {
		return allocator->Allocate(allocator, size, alignment, outAllocated);
	}

	void Deallocate(IAllocator* allocator, void* mem) {
		allocator->Deallocate(allocator, mem);
	}

	void AllocatorDeinit(IAllocator* allocator) {
		allocator->Deinit(allocator);
	}

	void* TagAllocate(ITagAllocator* allocator, MemTag tag, u64 size, u64 alignment) {
		return allocator->Allocate(allocator, tag, size, alignment, nullptr);
	}

	void* TagAllocate(ITagAllocator* allocator, MemTag tag, u64 size, u64 alignment, u64* outAllocated) {
		return allocator->Allocate(allocator, tag, size, alignment, outAllocated);
	}

	void TagDeallocate(ITagAllocator* allocator, MemTag tag) {
		allocator->Deallocate(allocator, tag);
	}

	void TagAllocatorDeinit(ITagAllocator* allocator) {
		allocator->Deinit(allocator);
	}
}