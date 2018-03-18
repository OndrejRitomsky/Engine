#include "SingleTagProxyAllocator.h"

#include "Core/CoreAssert.h"

#include "Core/Allocator/IAllocator.inl"

#include "Core/TagAllocator/SafeTaggedBlockAllocator.h"

namespace core {
	//---------------------------------------------------------------------------
	SingleTagProxyAllocator::SingleTagProxyAllocator(SafeTaggedBlockAllocator* backingAllocator, AllocationTag tag) :
		_backingAllocator(backingAllocator),
		_tag(tag) {
	}

	//---------------------------------------------------------------------------
	void* SingleTagProxyAllocator::Allocate(u64 size, u64 alignment) {
		u64 blockAlignment = _backingAllocator->GetBlockAlignment();
		u64 blockSize = _backingAllocator->GetBlockSize();

		// This should be used only as proxy for other allocators which should further divide the block
		// The checks is made if space is not wasted or too big alignment used
		ASSERT(alignment <= blockAlignment); 
		// ASSERT(size >= blockSize / 2.);  -- Curently using the overriden method

		if (size <= blockSize)
			return _backingAllocator->AllocateTaggedBlock(_tag);
		else 
			return _backingAllocator->AllocateTaggedMultiBlock(_tag, size / blockSize + 1);
	}

	//---------------------------------------------------------------------------
	void* SingleTagProxyAllocator::Allocate(u64 size, u64 alignment, u64& outAllocatedSize) {
		u64 blockSize = _backingAllocator->GetBlockSize();
		outAllocatedSize = (size / blockSize + 1) * blockSize;
		return Allocate(size, alignment);
	}

	//---------------------------------------------------------------------------
	void SingleTagProxyAllocator::Deallocate(void* address) {
		ASSERT(false);
		// Game engine should deallocate by TAG
	}

}