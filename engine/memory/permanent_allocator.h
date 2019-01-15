#pragma once


#include <core/allocator/allocate.h>
#include <core/common/placement.h>

namespace core {
	struct IAllocator;
}

namespace eng {
	// Used for engine allocation of modules and its` subcomponents or other permanent data
	// will be deallocated only on engine destruction
	struct PermanentAllocator {
		struct Block {
			void* start;
			void* current;
			Block* next;
			u64 sizeLeft;
		};

		u64 _minBlockSize;
		core::IAllocator* _allocator;
		Block* _block;
		Block _firstBlock;
	};


	void PermanentAllocatorInit(PermanentAllocator* perma, core::IAllocator* allocator, u64 minBlockSize);

	void PermanentAllocatorDeinit(PermanentAllocator* perma);

	void* PermanentAllocatorReserve(PermanentAllocator* perma, u64 size, u64 alignment);

#define PermanentNew(perma, Type) Placement(PermanentAllocatorReserve(perma, sizeof(Type), alignof(Type))) Type
}
