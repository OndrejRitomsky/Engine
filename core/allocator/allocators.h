#pragma once

#include "iallocators.h"

#include "../common/types.h"

namespace core {
	struct HeapAllocator;
	struct SingleTagProxyAllocator;
	struct SafeTaggedBlockAllocator;

	IAllocator* HeapAllocatorInit(HeapAllocator* heapAllocator);

	IAllocator* SingleTagProxyAllocatorInit(SingleTagProxyAllocator* allocator, ITagAllocator* backingAllocator, MemTag tag);

	ITagAllocator* SafeTaggedBlockAllocatorInit(SafeTaggedBlockAllocator* allocator, IAllocator* backingAllocator, u64 blockSize, u64 blockAlignment, MemTag invalidTag = U64MAX);

	struct HeapAllocator {
		IAllocator allocator;
		u32 _allocationsCount;
	};

	struct SingleTagProxyAllocator {
		IAllocator allocator;
		ITagAllocator* _backingAllocator;
		MemTag _tag;
	};


	struct SafeTaggedBlockAllocator {
		struct BlockMeta;

		struct BlockInfo {
			u64 size;
			u64 capacity;
			u64 blockCount;
			void* data;

			u64* occupied;
			u64* successiveBlocks;
			struct BlockMeta* meta;
		};

		ITagAllocator allocator;

		BlockInfo _data;

		MemTag _invalidTag;
		u64 _blockSize;
		u64 _blockAlignment;
		u64 _freeFirst; // todo inplace linked list

		IAllocator* _backingAllocator;
	};

}

