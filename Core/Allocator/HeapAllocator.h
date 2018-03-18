#pragma once

#include "Core/Allocator/IAllocator.h"

namespace core {

	class HeapAllocator : public IAllocator {
	public:
		HeapAllocator();
		HeapAllocator(const HeapAllocator& allocator) = delete;	
		HeapAllocator operator=(const HeapAllocator& allocator) = delete;
		~HeapAllocator();

		virtual void* Allocate(u64 size, u64 allignment) override;
		virtual void Deallocate(void* address) override;

	private:
		u32 _allocationsCount;
	};
}