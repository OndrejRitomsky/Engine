#pragma once

#include "Core/Allocator/IAllocator.h"

namespace core {

	class HeapAllocator : public IAllocator {
	public:
		HeapAllocator();
		HeapAllocator(const HeapAllocator& allocator) = delete;	
		HeapAllocator operator=(const HeapAllocator& allocator) = delete;
		~HeapAllocator();

		void* Allocate(u64 size, u64 alignment, u64* outAllocated);
		void Deallocate(void* address);
		void Deinit();

	private:
		u32 _allocationsCount;
	};
}