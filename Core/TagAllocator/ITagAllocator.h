#pragma once

#include "Core/Common/Types.h"

#include "Core/TagAllocator/MemTag.h"

namespace core {
	class ITagAllocator {
	public:
		void* AllocateWithTag(const MemTag& tag, u64 size, u64 alignment, u64* outAllocated);

		void DeallocateAllWithTag(const MemTag& tag);

		void Deinit();

	public:
		typedef void*(ITagAllocator::*AllocateWithTagFunctin)(const MemTag& tag, u64 size, u64 alignment, u64* outAllocated);
		typedef void(ITagAllocator::*DeallocateAllWithTagFunction)(const MemTag& tag);
		typedef void(ITagAllocator::*DeinitFunction)();

		AllocateWithTagFunctin _AllocateWithTag;
		DeallocateAllWithTagFunction _DeallocateAllWithTag;
		DeinitFunction _Deinit;
	};
}


