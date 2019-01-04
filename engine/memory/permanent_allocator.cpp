#include "permanent_allocator.h"

#include <core/common/types.h>
#include <core/common/utility.h>
#include <core/common/pointer.h>


namespace eng {
	void PermanentAllocatorInit(PermanentAllocator* perma, core::IAllocator* allocator, u64 minBlockSize) {
		perma->_allocator = allocator;
		perma->_block = &perma->_firstBlock;
		perma->_minBlockSize = minBlockSize;
		
		perma->_firstBlock.next = nullptr;
		perma->_firstBlock.start = core::Allocate(allocator, minBlockSize, 1);
		perma->_firstBlock.current = perma->_firstBlock.start;
		perma->_firstBlock.sizeLeft = minBlockSize;
	}

	void PermanentAllocatorDeinit(PermanentAllocator* perma) {
		PermanentAllocator::Block* block = perma->_firstBlock.next;
		while (block) {
			PermanentAllocator::Block* next = block->next;
			core::Deallocate(perma->_allocator, block);
			block = next;
		}

		core::Deallocate(perma->_allocator, perma->_firstBlock.start);
	}

	void* PermanentAllocatorReserve(PermanentAllocator* perma, u64 size, u64 alignment) {
		u64 needed = alignment + size;
		if (needed > perma->_block->sizeLeft) {
			u64 blockSize = core::Max(needed, perma->_minBlockSize);
			PermanentAllocator::Block* newBlock = (PermanentAllocator::Block*) core::Allocate(perma->_allocator, 
				sizeof(PermanentAllocator::Block) + blockSize, alignof(PermanentAllocator::Block));
			newBlock->sizeLeft = blockSize;
			newBlock->start = (void*) (newBlock + 1);
			newBlock->current = newBlock->start;

			perma->_block->next = newBlock;
			perma->_block = newBlock;
		}

		char* data = (char*) core::PointerAlign(perma->_block->current, alignment);

		ASSERT(perma->_block->sizeLeft >= (u64) ((data) -((char*) perma->_block->current)));
		perma->_block->sizeLeft -= (data) -((char*) perma->_block->current);
		perma->_block->current = data + size + 1;

		return (void*) data;
	}
}