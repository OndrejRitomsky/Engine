#include "../allocator/allocators.h"

#include "allocate.h"
#include "allocators.h"

#include "../common/debug.h"
#include "../common/pointer.h"
#include "../algorithm/cstring.h"

namespace core {
	static const u64 INVALID_INDEX = U64MAX;
	static const u64 DEFAULT_ALIGNMENT = 8;
	
	// @TODO move bit array stuff to separate, base struct

	struct SafeTaggedBlockAllocator::BlockMeta {
		MemTag tag;
		void* block;
	};

	struct FreeBlockHeader {
		u64 nextIndex;
	};

	static const u64 DIV_64_SHIFT = 6; // = /64 
	static const u64 MOD_64_AND = (1 << 6) - 1; // = % 64
	static const u64 BITS_IN_FIELD = sizeof(u64) * 8;


	inline static u64 BitFieldOffset(u64 index) {
		// Bits are arranged left to right (higher number first)
		return BITS_IN_FIELD - 1 - (index & MOD_64_AND);
	}

	inline static u64 BitArrayIs(void* data, u64 index) {
		u64* field = (u64*) data + (index >> DIV_64_SHIFT);
		return *field & (1llu << BitFieldOffset(index));
	}

	inline static void BitArraySet(void* data, u64 index) {
		u64* field = (u64*) data + (index >> DIV_64_SHIFT);
		*field |= 1llu << BitFieldOffset(index);
	}

	inline static void BitArrayClear(void* data, u64 index) {
		u64* field = (u64*) data + (index >> DIV_64_SHIFT);
		*field &= ~(1llu << BitFieldOffset(index));
	}

	void* AllocateTaggedBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag);

	void* AllocateTaggedMultiBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 blockCount);

	u64 NextFreeIndex(SafeTaggedBlockAllocator* tagAllocator, u64 fromIndex);

	void SetNextFreeIndex(SafeTaggedBlockAllocator* tagAllocator, u64 fromIndex, u64 toIndex);

	void* AllocateMultiBlockAtIndex(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 blockCount, u64 index);

	void* AllocateNewMultiBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 blockCount);

	void SetUpBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 index, void* data, u64 successiveBlocks);

	bool EnsureCapacity(SafeTaggedBlockAllocator* tagAllocator, u64 blocksNeeded);


	static void* ITagAllocatorAllocate(ITagAllocator* allocator, const MemTag& tag, u64 size, u64 alignment, u64* outAllocated) {
		SafeTaggedBlockAllocator* tagAllocator = (SafeTaggedBlockAllocator*) allocator;
		u64 blockSize = tagAllocator->_blockSize;

		if (outAllocated)
			*outAllocated = size % blockSize == 0 ? size : (size / blockSize + 1) * blockSize;

		u64 blockAlignment = tagAllocator->_blockAlignment;
		ASSERT(alignment <= blockAlignment);

		if (size <= blockSize)
			return AllocateTaggedBlock(tagAllocator, tag);
		else
			return AllocateTaggedMultiBlock(tagAllocator, tag, size / blockSize + 1);
	}

	static void ITagAllocatorDeallocate(ITagAllocator* allocator, const MemTag& tag) {
		SafeTaggedBlockAllocator* tagAllocator = (SafeTaggedBlockAllocator*) allocator;
		SafeTaggedBlockAllocator::BlockMeta* blockMeta = tagAllocator->_data.meta;

		for (u64 i = 0; i < tagAllocator->_data.size; ++i, ++blockMeta) {
			if (blockMeta->tag == tag) {
				blockMeta->tag = tagAllocator->_invalidTag;

				ASSERT(BitArrayIs(tagAllocator->_data.occupied, i));
				BitArrayClear(tagAllocator->_data.occupied, i);

				FreeBlockHeader* currentBlock = (FreeBlockHeader*) blockMeta->block;
				currentBlock->nextIndex = tagAllocator->_freeFirst;
				tagAllocator->_freeFirst = i;
			}
		}
	}

	static void ITagAllocatorDeinit(ITagAllocator* allocator) {
		SafeTaggedBlockAllocator* tagAllocator = (SafeTaggedBlockAllocator*) allocator;

		// @TODO this neeeds to be decided, if allocations can be permanent or everything has to be cleared by tag!!!
		for (u64 i = 0; i < tagAllocator->_data.size; ++i) {
			ASSERT(!BitArrayIs(tagAllocator->_data.occupied, i) || tagAllocator->_data.meta[i].tag == tagAllocator->_invalidTag);
		}

		bool sequence = false;
		bool wasSequence = false;
		for (u64 i = 0; i < tagAllocator->_data.size; ++i) {
			sequence = tagAllocator->_data.successiveBlocks[i] > 1;

			if (!wasSequence)
				Deallocate(tagAllocator->_backingAllocator, tagAllocator->_data.meta[i].block);

			wasSequence = sequence;
		}

		Deallocate(tagAllocator->_backingAllocator, tagAllocator->_data.data);

		tagAllocator->_data.size = 0;
	}

	ITagAllocator* SafeTaggedBlockAllocatorInit(SafeTaggedBlockAllocator* allocator, IAllocator* backingAllocator, u64 blockSize, u64 blockAlignment, MemTag invalidTag) {
		allocator->_data.size = 0;
		allocator->_data.capacity = 0;
		allocator->_data.blockCount = 0;
		allocator->_data.data = nullptr;
		allocator->_data.occupied = nullptr;
		allocator->_data.successiveBlocks = nullptr;
		allocator->_data.meta = nullptr;

		allocator->_backingAllocator = backingAllocator;
		allocator->_freeFirst = INVALID_INDEX;
		allocator->_blockSize = blockSize;
		allocator->_blockAlignment = blockAlignment;
		allocator->_invalidTag = invalidTag;

		ASSERT(backingAllocator);
		ASSERT(blockSize >= sizeof(FreeBlockHeader));

		allocator->allocator.Allocate = ITagAllocatorAllocate;
		allocator->allocator.Deallocate = ITagAllocatorDeallocate;
		allocator->allocator.Deinit = ITagAllocatorDeinit;
		return &allocator->allocator;
	}



	static void* AllocateTaggedBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag) {
		void* result = nullptr;

		if (tagAllocator->_freeFirst != INVALID_INDEX) {
			u64 blockIndex = tagAllocator->_freeFirst;
			
			result = tagAllocator->_data.meta[blockIndex].block;
			tagAllocator->_data.meta[blockIndex].tag = tag;
			tagAllocator->_freeFirst = NextFreeIndex(tagAllocator, blockIndex);

			ASSERT(!BitArrayIs(tagAllocator->_data.occupied, blockIndex));
			BitArraySet(tagAllocator->_data.occupied, blockIndex);
		}
		else {
			result = Allocate(tagAllocator->_backingAllocator, tagAllocator->_blockSize, DEFAULT_ALIGNMENT);

			if (result) {
				if (EnsureCapacity(tagAllocator, 1)) {
					SetUpBlock(tagAllocator, tag, tagAllocator->_data.size, result, 1);
					tagAllocator->_data.size++;
				} 
				else {
					Deallocate(tagAllocator->_backingAllocator, result);
					result = nullptr;
				}
			}
		}

		return result;
	}

	static u64 NextFreeIndex(SafeTaggedBlockAllocator* tagAllocator, u64 fromIndex) {
		FreeBlockHeader* freeHeader = (FreeBlockHeader*) tagAllocator->_data.meta[fromIndex].block;
		return freeHeader->nextIndex;
	}

	static void SetNextFreeIndex(SafeTaggedBlockAllocator* tagAllocator, u64 fromIndex, u64 toIndex) {
		FreeBlockHeader* freeHeader = (FreeBlockHeader*) tagAllocator->_data.meta[fromIndex].block;
		freeHeader->nextIndex = toIndex;
	}

	static void* AllocateMultiBlockAtIndex(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 blockCount, u64 index) {
		ASSERT(index + blockCount <= tagAllocator->_data.size);
		
		// Set used
		u64 size = index + blockCount;
		for (u64 i = index; i < size; ++i) {
			tagAllocator->_data.meta[i].tag = tag;
			BitArraySet(tagAllocator->_data.occupied, i);
		}

		{
		/*	u64 dbg = _freeFirst;
			while (dbg != INVALID_INDEX) {
				printf("%llu ", dbg);
				dbg = NextFreeIndex(dbg);
			}
			printf("\n");*/
		}

		// Free list fix
		u64 preBlockIndex = tagAllocator->_freeFirst;
		u64 blockIndex = tagAllocator->_freeFirst;
		u64 nextIndex = INVALID_INDEX;

		while (blockIndex != INVALID_INDEX) {
			nextIndex = NextFreeIndex(tagAllocator, blockIndex);

			if (index <= blockIndex && blockIndex < index + blockCount) {
				SetNextFreeIndex(tagAllocator, preBlockIndex, nextIndex);
			}
			else {
				preBlockIndex = blockIndex;
			}

			blockIndex = nextIndex;
		}

		if (index <= tagAllocator->_freeFirst && tagAllocator->_freeFirst < index + blockCount)
			tagAllocator->_freeFirst = NextFreeIndex(tagAllocator, tagAllocator->_freeFirst);

		
		{
			/*u64 dbg = _freeFirst;
			while (dbg != INVALID_INDEX) {
				printf("%llu ", dbg);
				dbg = NextFreeIndex(dbg);
			}
			printf("\n");*/
		}

		return tagAllocator->_data.meta[index].block;
	}

	static void* AllocateNewMultiBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 blockCount) {
		void *result = Allocate(tagAllocator->_backingAllocator, tagAllocator->_blockSize * blockCount, DEFAULT_ALIGNMENT);

		if (result) {
			if (EnsureCapacity(tagAllocator, blockCount)) {
				u64 index = tagAllocator->_data.size;

				for (u64 i = 0; i < blockCount; ++i, ++index)
					SetUpBlock(tagAllocator, tag, index, PointerAdd(result, i * tagAllocator->_blockSize), blockCount - i);

				tagAllocator->_data.size += blockCount;
			}
			else {
				Deallocate(tagAllocator->_backingAllocator, result);
				result = nullptr;
			}
		}

		return result;
	}

	static void SetUpBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 index, void* data, u64 successiveBlocks) {
		BitArraySet(tagAllocator->_data.occupied, index);

		SafeTaggedBlockAllocator::BlockMeta& meta = tagAllocator->_data.meta[index];
		meta.tag = tag;
		meta.block = data;

		tagAllocator->_data.successiveBlocks[index] = successiveBlocks;
	}

	static void* AllocateTaggedMultiBlock(SafeTaggedBlockAllocator* tagAllocator, MemTag tag, u64 blockCount) {
		// This allocation is slow compared to single block allocation
		// Iterating over all allocated block and iterating over free list

		ASSERT(blockCount > 0);
		if (blockCount == 1)
			return AllocateTaggedBlock(tagAllocator, tag);

		static const u64 FREE_NOT_FOUND = U64MAX;

		u64 bestIndex = FREE_NOT_FOUND;
		u64 bestFit = FREE_NOT_FOUND;
		u64 free = 0;

		for (u64 i = 0; i < tagAllocator->_data.size; ++i) {
			if (BitArrayIs(tagAllocator->_data.occupied, i)) {
				// Enough free blocks AND
				// its the best fit and its the last best one
				if (free >= blockCount && free <= bestFit) {
					bestIndex = i - blockCount; // the last free blocks in free sequence
					bestFit = free; 
				}
				free = 0;
			} 
			else {
				// start sequence or continue (already big enough sequence or ch
				ASSERT(tagAllocator->_data.successiveBlocks[i] > 0);
				bool endOfGoodSequence = tagAllocator->_data.successiveBlocks[i] == 1;
				
				// continue already big enough sequence or
				//		start or continue sequence if its successive blocks
				if (free > blockCount || tagAllocator->_data.successiveBlocks[i] >= blockCount - free) {
					++free; 
				}
				else {
					endOfGoodSequence = false; // end of successive blocks
					free = 0;
				}
				// end of successiveBlocks
				if (endOfGoodSequence) {
					if (free >= blockCount && free <= bestFit) {
						bestIndex = i - blockCount + 1; // the last free blocks in free sequence
						bestFit = free;
					}
					free = 0;
				}

			}
		}

		// free block sequence can still be "open"
		if (free >= blockCount && free <= bestFit) {
			bestIndex = tagAllocator->_data.size - blockCount; // the last free blocks in free sequence
			bestFit = free;
		}

		if (bestIndex == FREE_NOT_FOUND)
			return AllocateNewMultiBlock(tagAllocator, tag, blockCount);
		else
			return AllocateMultiBlockAtIndex(tagAllocator, tag, blockCount, bestIndex);
	}

	static bool EnsureCapacity(SafeTaggedBlockAllocator* tagAllocator, u64 blocksNeeded) {
		u64 left = tagAllocator->_data.capacity - tagAllocator->_data.size;
		if (blocksNeeded > left) {
			// Count in 8 blocks, Using bit array for occupied info (8 in byte)
			const u64 eightInfoBlocksSize = 8 * (sizeof(SafeTaggedBlockAllocator::BlockMeta) + sizeof(u64)) + sizeof(u8);

			u64 infoBlocksInBlock = (tagAllocator->_blockSize / eightInfoBlocksSize) * 8 - 1; // one blockInfo is reserve 
			u64 infoBlocksToAllocate = tagAllocator->_data.size + blocksNeeded;

			u64 blocksToAllocate = infoBlocksToAllocate / infoBlocksInBlock + 1; // ONLY CONSTANT INCREASE, ONE BLOCK SHOULD BE ALREADY BIG ENOUGH

			void* newData = Allocate(tagAllocator->_backingAllocator, blocksToAllocate * tagAllocator->_blockSize, alignof(u64));
			ASSERT(newData);
			if (!newData)
				return false;

			if (tagAllocator->_data.data) {
				Memcpy(newData, tagAllocator->_data.data, tagAllocator->_data.blockCount * tagAllocator->_blockSize);
				Deallocate(tagAllocator->_backingAllocator, tagAllocator->_data.data); // @TODO dont deallocate add to pool
			}

			tagAllocator->_data.capacity   = blocksToAllocate * infoBlocksInBlock;
			tagAllocator->_data.data       = newData;
			tagAllocator->_data.blockCount = blocksToAllocate;

			tagAllocator->_data.occupied         = (u64*) tagAllocator->_data.data;
			tagAllocator->_data.successiveBlocks = (u64*) (tagAllocator->_data.occupied + tagAllocator->_data.capacity / BITS_IN_FIELD + 1);
			tagAllocator->_data.meta             = (SafeTaggedBlockAllocator::BlockMeta*) (tagAllocator->_data.successiveBlocks + tagAllocator->_data.capacity);

			ASSERT((u8*)(tagAllocator->_data.occupied + tagAllocator->_data.capacity / BITS_IN_FIELD + 1) <= (u8*) tagAllocator->_data.successiveBlocks);
			ASSERT((u8*)(tagAllocator->_data.successiveBlocks + tagAllocator->_data.capacity) <= (u8*) tagAllocator->_data.meta);
			ASSERT((u8*)(tagAllocator->_data.meta + tagAllocator->_data.capacity) <= (u8*) tagAllocator->_data.data + tagAllocator->_data.blockCount * tagAllocator->_blockSize);

			Memset(tagAllocator->_data.occupied, 0, sizeof(u64) * tagAllocator->_data.capacity);
		}

		return true;
	}


}