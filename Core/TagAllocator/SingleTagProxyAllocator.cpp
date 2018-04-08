#include "SingleTagProxyAllocator.h"

#include "Core/Common/Assert.h"

#include "Core/Allocator/IAllocator.inl"

#include "Core/TagAllocator/ITagAllocator.h"

namespace core {
	//---------------------------------------------------------------------------
	SingleTagProxyAllocator::SingleTagProxyAllocator() :
		_backingAllocator(nullptr),
		_tag(0) {

		_interface._Allocate = (IAllocator::AllocateFunction) (&SingleTagProxyAllocator::Allocate);
		_interface._Deallocate = (IAllocator::DeallocateFunction) (&SingleTagProxyAllocator::Deallocate);
		_interface._Deinit = (IAllocator::DeinitFunction) (&SingleTagProxyAllocator::Deinit);
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