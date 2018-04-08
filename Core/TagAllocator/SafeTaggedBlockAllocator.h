#pragma once

#include "Core/Common/Types.h"
#include "Core/Common/TypeLimits.h"

#include "Core/TagALlocator/MemTag.h"
#include "Core/TagALlocator/ITagAllocator.h"

namespace core {

	class IAllocator;

	// @TODO its not safe yet
	class SafeTaggedBlockAllocator {
	public:
		SafeTaggedBlockAllocator();
		SafeTaggedBlockAllocator(const SafeTaggedBlockAllocator& oth) = delete;
		SafeTaggedBlockAllocator& operator=(const SafeTaggedBlockAllocator& rhs) = delete;

		~SafeTaggedBlockAllocator();

		void Init(IAllocator* backingAllocator, u64 blockSize, u64 blockAlignment, MemTag invalidTag = U64MAX);

		void* AllocateWithTag(const MemTag& tag, u64 size, u64 alignment, u64* outAllocated);

		void DeallocateAllWithTag(const MemTag& tag);

		void Deinit();

		u64 GetBlockSize();
		u64 GetBlockAlignment();

		ITagAllocator* TagAllocator();

	private:
		void* AllocateTaggedBlock(MemTag tag);
		void* AllocateTaggedMultiBlock(MemTag tag, u64 blockCount);

		bool EnsureCapacity(u64 blocksNeeded);

		void* AllocateMultiBlockAtIndex(MemTag tag, u64 blockCount, u64 index);
		void* AllocateNewMultiBlock(MemTag tag, u64 blockCount);
		void SetUpBlock(MemTag tag, u64 index, void* data, u64 successiveBlocks);

		u64 NextFreeIndex(u64 fromIndex);
		void SetNextFreeIndex(u64 fromIndex, u64 toIndex);

	private:
		ITagAllocator _interface;

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

		MemTag _invalidTag;
		u64 _blockSize;
		u64 _blockAlignment;
		u64 _freeFirst; // todo inplace linked list

		IAllocator* _backingAllocator;
	};
}
