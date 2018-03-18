#pragma once

#include "Core/Types.h"

#include "Core/TagALlocator/AllocationTag.h"

namespace core {

	class IAllocator;

	class SafeTaggedBlockAllocator {
	public:
		SafeTaggedBlockAllocator();
		SafeTaggedBlockAllocator(const SafeTaggedBlockAllocator& oth) = delete;
		SafeTaggedBlockAllocator& operator=(const SafeTaggedBlockAllocator& rhs) = delete;

		~SafeTaggedBlockAllocator();

		void Init(IAllocator* backingAllocator, u64 blockSize, u64 blockAlignment, AllocationTag invalidTag = U64MAX);

		void* AllocateTaggedBlock(AllocationTag tag);
		void* AllocateTaggedMultiBlock(AllocationTag tag, u64 blockCount);

		void ClearByTag(AllocationTag tag);

		u64 GetBlockSize();
		u64 GetBlockAlignment();

	private:
		bool EnsureCapacity(u64 blocksNeeded);

		void* AllocateMultiBlockAtIndex(AllocationTag tag, u64 blockCount, u64 index);
		void* AllocateNewMultiBlock(AllocationTag tag, u64 blockCount);
		void SetUpBlock(AllocationTag tag, u64 index, void* data, u64 successiveBlocks);

		u64 NextFreeIndex(u64 fromIndex);
		void SetNextFreeIndex(u64 fromIndex, u64 toIndex);

	private:
		struct BlockMeta;

		struct BlockInfo {
			u64 size;
			u64 capacity;
			u64 blockCount;
			void* data;

			u64* occupied;
			u64* successiveBlocks;
			BlockMeta* meta;
		};

		BlockInfo _data;

		AllocationTag _invalidTag;
		u64 _blockSize;
		u64 _blockAlignment;
		u64 _freeFirst;

		IAllocator* _backingAllocator;
	};
}
