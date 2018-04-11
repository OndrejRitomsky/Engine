#include "SafeTaggedBlockAllocator.h"

#include "Core/Common/Assert.h"
#include "Core/Allocator/IAllocator.h"
#include "Core/Allocator/IAllocator.inl"

#include "Core/Common/Pointer.h"
#include "Core/Algorithm/Memory.h"
#include "Core/Common/TypeLimits.h"

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

	//-------------------------------------------------------------------------
	inline static u64 BitArrayIs(void* data, u64 index) {
		u64* field = (u64*) data + (index >> DIV_64_SHIFT);
		return *field & (1llu << BitFieldOffset(index));
	}

	//-------------------------------------------------------------------------
	inline static void BitArraySet(void* data, u64 index) {
		u64* field = (u64*) data + (index >> DIV_64_SHIFT);
		*field |= 1llu << BitFieldOffset(index);
	}

	//-------------------------------------------------------------------------
	inline static void BitArrayClear(void* data, u64 index) {
		u64* field = (u64*) data + (index >> DIV_64_SHIFT);
		*field &= ~(1llu << BitFieldOffset(index));
	}

	//-------------------------------------------------------------------------
	SafeTaggedBlockAllocator::SafeTaggedBlockAllocator() :
		_backingAllocator(nullptr),
		_freeFirst(INVALID_INDEX),
		_blockSize(0),
		_blockAlignment(0),
		_invalidTag(0) {

		_data.size = 0;
		_data.capacity = 0;
		_data.blockCount = 0;
		_data.data = nullptr;
		_data.occupied = nullptr;
		_data.successiveBlocks = nullptr;
		_data.meta = nullptr;

		// EnsureCapacity(1);
	}

	//-------------------------------------------------------------------------
	SafeTaggedBlockAllocator::~SafeTaggedBlockAllocator() {
		Deinit();
	}

	//-------------------------------------------------------------------------
	void SafeTaggedBlockAllocator::Init(IAllocator* backingAllocator, u64 blockSize, u64 blockAlignment, MemTag invalidTag) {
		_backingAllocator = backingAllocator;
		_freeFirst = INVALID_INDEX;
		_blockSize = blockSize;
		_blockAlignment = blockAlignment;
		_invalidTag = invalidTag;

		ASSERT(backingAllocator);
		ASSERT(blockSize >= sizeof(FreeBlockHeader));

		_AllocateWithTag = static_cast<ITagAllocator::AllocateWithTagFunctin>(&SafeTaggedBlockAllocator::AllocateWithTag);
		_DeallocateAllWithTag = static_cast<ITagAllocator::DeallocateAllWithTagFunction>(&SafeTaggedBlockAllocator::DeallocateAllWithTag);
		_Deinit = static_cast<ITagAllocator::DeinitFunction>(&SafeTaggedBlockAllocator::Deinit);
	}

	//-------------------------------------------------------------------------
	void* SafeTaggedBlockAllocator::AllocateWithTag(const MemTag& tag, u64 size, u64 alignment, u64* outAllocated) {
		u64 blockSize = GetBlockSize();

		if (outAllocated)
			*outAllocated = size % blockSize == 0 ? size : (size / blockSize + 1) * blockSize;

		u64 blockAlignment = GetBlockAlignment();
		ASSERT(alignment <= blockAlignment);

		if (size <= blockSize)
			return AllocateTaggedBlock(tag);
		else
			return AllocateTaggedMultiBlock(tag, size / blockSize + 1);
	}

	//-------------------------------------------------------------------------
	void SafeTaggedBlockAllocator::DeallocateAllWithTag(const MemTag& tag) {
		BlockMeta* blockMeta = _data.meta;

		for (u64 i = 0; i < _data.size; ++i, ++blockMeta) {
			if (blockMeta->tag == tag) {
				blockMeta->tag = _invalidTag;

				ASSERT(BitArrayIs(_data.occupied, i));
				BitArrayClear(_data.occupied, i);

				FreeBlockHeader* currentBlock = (FreeBlockHeader*) blockMeta->block;
				currentBlock->nextIndex = _freeFirst;
				_freeFirst = i;
			}
		}
	}

	//-------------------------------------------------------------------------
	void SafeTaggedBlockAllocator::Deinit() {
		// @TODO this neeeds to be decided, if allocations can be permanent or everything has to be cleared by tag!!!
		for (u64 i = 0; i < _data.size; ++i) {
			ASSERT(!BitArrayIs(_data.occupied, i));
		}

		bool sequence = false;
		bool wasSequence = false;
		for (u64 i = 0; i < _data.size; ++i) {
			sequence = _data.successiveBlocks[i] > 1;

			if (!wasSequence)
				_backingAllocator->Deallocate(_data.meta[i].block);

			wasSequence = sequence;
		}

		_backingAllocator->Deallocate(_data.data);

		_data.size = 0;
	}

	//-------------------------------------------------------------------------
	void* SafeTaggedBlockAllocator::AllocateTaggedBlock(MemTag tag) {
		void* result = nullptr;

		if (_freeFirst != INVALID_INDEX) {
			u64 blockIndex = _freeFirst;
			
			result = _data.meta[blockIndex].block;
			_freeFirst = NextFreeIndex(blockIndex);

			ASSERT(!BitArrayIs(_data.occupied, blockIndex));
			BitArraySet(_data.occupied, blockIndex);
		}
		else {
			result = _backingAllocator->Allocate(_blockSize, DEFAULT_ALIGNMENT);

			if (result) {
				if (EnsureCapacity(1)) {
					SetUpBlock(tag, _data.size, result, 1);
					++_data.size;
				} 
				else {
					_backingAllocator->Deallocate(result);
					result = nullptr;
				}
			}
		}

		return result;
	}

	//-------------------------------------------------------------------------
	u64 SafeTaggedBlockAllocator::NextFreeIndex(u64 fromIndex) {
		FreeBlockHeader* freeHeader = (FreeBlockHeader*) _data.meta[fromIndex].block;
		return freeHeader->nextIndex;
	}

	//-------------------------------------------------------------------------
	void SafeTaggedBlockAllocator::SetNextFreeIndex(u64 fromIndex, u64 toIndex) {
		FreeBlockHeader* freeHeader = (FreeBlockHeader*) _data.meta[fromIndex].block;
		freeHeader->nextIndex = toIndex;
	}

	//-------------------------------------------------------------------------
	void* SafeTaggedBlockAllocator::AllocateMultiBlockAtIndex(MemTag tag, u64 blockCount, u64 index) {
		ASSERT(index + blockCount <= _data.size);
		
		// Set used
		u64 size = index + blockCount;
		for (u64 i = index; i < size; ++i) {
			_data.meta[i].tag = tag;
			BitArraySet(_data.occupied, i);
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
		u64 preBlockIndex = _freeFirst;
		u64 blockIndex = _freeFirst;
		u64 nextIndex = INVALID_INDEX;

		while (blockIndex != INVALID_INDEX) {
			nextIndex = NextFreeIndex(blockIndex);

			if (index <= blockIndex && blockIndex < index + blockCount) {
				SetNextFreeIndex(preBlockIndex, nextIndex);
			}
			else {
				preBlockIndex = blockIndex;
			}

			blockIndex = nextIndex;
		}

		if (index <= _freeFirst && _freeFirst < index + blockCount)
			_freeFirst = NextFreeIndex(_freeFirst);

		
		{
			/*u64 dbg = _freeFirst;
			while (dbg != INVALID_INDEX) {
				printf("%llu ", dbg);
				dbg = NextFreeIndex(dbg);
			}
			printf("\n");*/
		}

		return _data.meta[index].block;
	}
	//-------------------------------------------------------------------------
	void* SafeTaggedBlockAllocator::AllocateNewMultiBlock(MemTag tag, u64 blockCount) {
		void *result = _backingAllocator->Allocate(_blockSize * blockCount, DEFAULT_ALIGNMENT);

		if (result) {
			if (EnsureCapacity(blockCount)) {
				u64 index = _data.size;

				for (u64 i = 0; i < blockCount; ++i, ++index)
					SetUpBlock(tag, index, mem::Add(result, i * _blockSize), blockCount - i);

				_data.size += blockCount;
			}
			else {
				_backingAllocator->Deallocate(result);
				result = nullptr;
			}
		}

		return result;
	}

	//-------------------------------------------------------------------------
	void SafeTaggedBlockAllocator::SetUpBlock(MemTag tag, u64 index, void* data, u64 successiveBlocks) {
		BitArraySet(_data.occupied, index);

		BlockMeta& meta = _data.meta[index];
		meta.tag = tag;
		meta.block = data;

		_data.successiveBlocks[index] = successiveBlocks;
	}

	//-------------------------------------------------------------------------
	void* SafeTaggedBlockAllocator::AllocateTaggedMultiBlock(MemTag tag, u64 blockCount) {
		// This allocation is slow compared to single block allocation
		// Iterating over all allocated block and iterating over free list

		ASSERT(blockCount > 0);
		if (blockCount == 1)
			return AllocateTaggedBlock(tag);

		static const u64 FREE_NOT_FOUND = U64MAX;

		u64 bestIndex = FREE_NOT_FOUND;
		u64 bestFit = FREE_NOT_FOUND;
		u64 free = 0;

		for (u64 i = 0; i < _data.size; ++i) {
			if (BitArrayIs(_data.occupied, i)) {
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
				ASSERT(_data.successiveBlocks[i] > 0);
				bool endOfGoodSequence = _data.successiveBlocks[i] == 1;
				
				// continue already big enough sequence or
				//		start or continue sequence if its successive blocks
				if (free > blockCount || _data.successiveBlocks[i] >= blockCount - free) {
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
			bestIndex = _data.size - blockCount; // the last free blocks in free sequence
			bestFit = free;
		}

		if (bestIndex == FREE_NOT_FOUND)
			return AllocateNewMultiBlock(tag, blockCount);
		else
			return AllocateMultiBlockAtIndex(tag, blockCount, bestIndex);
	}

	//-------------------------------------------------------------------------
	bool SafeTaggedBlockAllocator::EnsureCapacity(u64 blocksNeeded) {
		u64 left = _data.capacity - _data.size;
		if (blocksNeeded > left) {
			// Count in 8 blocks, Using bit array for occupied info (8 in byte)
			const u64 eightInfoBlocksSize = 8 * (sizeof(BlockMeta) + sizeof(u64)) + sizeof(u8);

			u64 infoBlocksInBlock = (_blockSize / eightInfoBlocksSize) * 8 - 1; // one blockInfo is reserve 
			u64 infoBlocksToAllocate = _data.size + blocksNeeded;

			u64 blocksToAllocate = infoBlocksToAllocate / infoBlocksInBlock + 1; // ONLY CONSTANT INCREASE, ONE BLOCK SHOULD BE ALREADY BIG ENOUGH

			void* newData = _backingAllocator->Allocate(blocksToAllocate * _blockSize, alignof(u64));
			ASSERT(newData);
			if (!newData)
				return false;

			if (_data.data) {
				Memcpy(newData, _data.data, _data.blockCount * _blockSize);
				_backingAllocator->Deallocate(_data.data); // @TODO dont deallocate add to pool
			}

			_data.capacity   = blocksToAllocate * infoBlocksInBlock;
			_data.data       = newData;
			_data.blockCount = blocksToAllocate;

			_data.occupied         = (u64*) _data.data;
			_data.successiveBlocks = (u64*) (_data.occupied + _data.capacity / BITS_IN_FIELD + 1);
			_data.meta             = (BlockMeta*) (_data.successiveBlocks + _data.capacity);

			ASSERT((u8*)(_data.occupied + _data.capacity / BITS_IN_FIELD + 1) <= (u8*) _data.successiveBlocks);
			ASSERT((u8*)(_data.successiveBlocks + _data.capacity) <= (u8*) _data.meta);
			ASSERT((u8*)(_data.meta + _data.capacity) <= (u8*) _data.data + _data.blockCount * _blockSize);

			Memset(_data.occupied, 0, sizeof(u64) * _data.capacity); 
		}

		return true;
	}

	//-------------------------------------------------------------------------
	u64 SafeTaggedBlockAllocator::GetBlockSize() {
		return _blockSize;
	}

	//-------------------------------------------------------------------------
	u64 SafeTaggedBlockAllocator::GetBlockAlignment() {
		return _blockAlignment;
	}
}