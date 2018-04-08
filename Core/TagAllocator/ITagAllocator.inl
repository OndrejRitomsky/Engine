#pragma once

#include "Core/TagAllocator/ITagAllocator.h"

namespace core {
	// @TODO this is not really a lot of code to have inlined file

	//---------------------------------------------------------------------------
	inline void* ITagAllocator::AllocateWithTag(const MemTag& tag, u64 size, u64 alignment, u64* outAllocated) {
		return (this->*_AllocateWithTag)(tag, size, alignment, outAllocated);
	}

	//---------------------------------------------------------------------------
	inline void ITagAllocator::DeallocateAllWithTag(const MemTag& tag) {
		return (this->*_DeallocateAllWithTag)(tag);
	}

	//---------------------------------------------------------------------------
	inline void ITagAllocator::Deinit() {
		return (this->*_Deinit)();
	}
}