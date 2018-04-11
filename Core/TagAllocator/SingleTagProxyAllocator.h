#pragma once

#include "Core/Common/Types.h"

#include "Core/Allocator/IAllocator.h"
#include "Core/TagAllocator/MemTag.h"


namespace core {
	class ITagAllocator;

	class SingleTagProxyAllocator : public IAllocator {
	public:
		SingleTagProxyAllocator();
		SingleTagProxyAllocator(const SingleTagProxyAllocator& oth) = delete;
		SingleTagProxyAllocator& operator=(const SingleTagProxyAllocator& rhs) = delete;

		void Init(ITagAllocator* backingAllocator, MemTag tag);

		~SingleTagProxyAllocator() = default;

		void* Allocate(u64 size, u64 allignment, u64* outAllocated);
		void Deallocate(void* address);
		void Deinit();

	private:
		ITagAllocator* _backingAllocator;
		MemTag _tag;
	};
}

