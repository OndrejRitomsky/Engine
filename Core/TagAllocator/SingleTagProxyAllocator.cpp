#include "SingleTagProxyAllocator.h"

#include "Core/Common/Assert.h"

#include "Core/Allocator/IAllocator.inl"

#include "Core/TagAllocator/ITagAllocator.h"

namespace core {
	//---------------------------------------------------------------------------
	SingleTagProxyAllocator::SingleTagProxyAllocator() :
		_backingAllocator(nullptr),
		_tag(0) {

		_Allocate = static_cast<IAllocator::AllocateFunction>(&SingleTagProxyAllocator::Allocate);
		_Deallocate = static_cast<IAllocator::DeallocateFunction>(&SingleTagProxyAllocator::Deallocate);
		_Deinit = static_cast<IAllocator::DeinitFunction>(&SingleTagProxyAllocator::Deinit);
	}

	//---------------------------------------------------------------------------
	void SingleTagProxyAllocator::Init(ITagAllocator* backingAllocator, MemTag tag) {
		ASSERT(!backingAllocator);
		_backingAllocator = backingAllocator;
		_tag = tag;
	}

	//---------------------------------------------------------------------------
	void* SingleTagProxyAllocator::Allocate(u64 size, u64 alignment, u64* outAllocated) {
		return _backingAllocator->AllocateWithTag(_tag, size, alignment, outAllocated);
	}

	//---------------------------------------------------------------------------
	void SingleTagProxyAllocator::Deallocate(void* address) {
		ASSERT(false);
	}

	//---------------------------------------------------------------------------
	void SingleTagProxyAllocator::Deinit() {

	}
}