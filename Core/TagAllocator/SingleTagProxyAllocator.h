#pragma once

#include "Core/Types.h"

#include "Core/Allocator/IAllocator.h"
#include "Core/TagAllocator/AllocationTag.h"


namespace core {
	class SafeTaggedBlockAllocator;

	class SingleTagProxyAllocator : public IAllocator
	{
	public:
		SingleTagProxyAllocator(SafeTaggedBlockAllocator* backingAllocator, AllocationTag tag);
		SingleTagProxyAllocator(const SingleTagProxyAllocator& oth) = delete;
		SingleTagProxyAllocator& operator=(const SingleTagProxyAllocator& rhs) = delete;

		~SingleTagProxyAllocator() override = default;

		void* Allocate(u64 size, u64 alignment, u64& outAllocatedSize);

		void* Allocate(u64 size, u64 alignment) override;
		void Deallocate(void* address) override;

	private:
		SafeTaggedBlockAllocator* _backingAllocator;
		AllocationTag _tag;
	};
}

