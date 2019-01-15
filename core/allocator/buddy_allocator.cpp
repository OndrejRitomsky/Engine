#include "buddy_allocator.h"

#include "../common/debug.h"

#include "../common/pointer.h"

// #include <memory.h>

// #include <malloc.h>  

/*
namespace core {

BuddyAllocator::BuddyAllocator(u32 leafSize, u32 levels, u32 dataAllignment) :
	_leafSize(leafSize),
	_totalAllocated(0),
	_allocationsCount(0),
	_levels(levels)
{
	ASSERT(_levels > 0 && _levels < MAX_LEVELS);
	ASSERT(IsPowerOfTwo(leafSize));

	_bufferSize = (1 << (_levels - 1)) * _leafSize;
	_start = _aligned_malloc(_bufferSize, dataAllignment);

	for (u32 i = 0; i < _levels; ++i) {
		_freeLists[i] = nullptr;
	}

	// all blocks
	u32 bytesForEntires = (1 << _levels) / ((u32) sizeof(u32));
	_occupiedBits = new u32[bytesForEntires]; // from backing allocator

	for (u32 i = 0; i < bytesForEntires; ++i) {
		Memset((void*) (_occupiedBits + i), 0, sizeof(u32));
	}

	_freeLists[0] = _start;
	*(void**) _start = 0;
}


BuddyAllocator::~BuddyAllocator()
{
	ASSERT(_totalAllocated == 0);
	ASSERT(_allocationsCount == 0);

	free(_start);
	delete _occupiedBits;
}

u32 BuddyAllocator::IndexInLevel(void* pointer, u32 level) {
	return (u32) ((u64) PointerSizeBetween(pointer, _start) / LevelSize(level));
}

// dynamic programming?, clean
void* BuddyAllocator::FindBlock(u32 level) {
	ASSERT(level < _levels); // -> out of memory

	void* freePointer = _freeLists[level];
	if (freePointer == nullptr) {
		freePointer = FindBlock(level - 1);
		_freeLists[level] = PointerAdd(freePointer, LevelSize(level));
		*(void**) _freeLists[level] = 0;
	}
	else {
		_freeLists[level] = *(void**) freePointer;
	}

	u32 levelIndex = IndexInLevel(freePointer, level);
	u32 index = GlobalIndex(level, levelIndex);

	u32 i = index / 64;
	u32 j = index % 64;

	_occupiedBits[i] = _occupiedBits[i] | (1 << (64 - j - 1));

	ASSERT(freePointer);
	return freePointer;
}

void* BuddyAllocator::Allocate(u64 size, u64 allignment)
{
	ASSERT(size <= _bufferSize);
	u32 level = FindLevel(size);
	void* p = FindBlock(level);
	ASSERT(PointerAlign(p, allignment) == p);

	_totalAllocated += LevelSize(level);
	_allocationsCount++;

	return p;
}

u32 BuddyAllocator::FindLevel(u32 size) {
	u32 levelSize = _bufferSize;
	u32 level = 0;

	while (levelSize > size && level < _levels) {
		levelSize /= 2;
		level++;
	}
	return level;
}

void BuddyAllocator::CleanAndMerge(u32 level, void* data) {
	ASSERT(level < _levels);

	u32 levelIndex = IndexInLevel(data, level);
	u32 index = GlobalIndex(level, levelIndex);

	u32 i = index / 64;
	u32 j = index % 64;

	ASSERT(_occupiedBits[i] & (1 << (64 - j - 1)));

	_occupiedBits[i] = _occupiedBits[i] & ~(1 << (64 - j - 1));

	if (level == 0) return;

	i32 sign = 1;
	u32 k = j + 1;

	if (levelIndex % 2 == 1) {
		k = j - 1;
		sign = -1;
	}
	if (_occupiedBits[i] & (1 << (64 - k - 1))) {
		// Buddy is occupied
		*(void**) data = _freeLists[level];
		_freeLists[level] = data;
	}
	else {
		// Buddy is empty

		u32 levelSize = LevelSize(level);
		void* buddy = sign == 1 ? PointerAdd(data, levelSize) : PointerSub(data, levelSize);
		if (_freeLists[level] == buddy) {
			_freeLists[level] = *(void**) buddy;
		}

		if (sign) {
			CleanAndMerge(level - 1, data);
		}
		else {
			CleanAndMerge(level - 1, buddy);
		}
	}
}

void BuddyAllocator::Deallocate(void* address, u64 size)
{
	if (address) {
		u32 level = FindLevel(size);
		CleanAndMerge(level, address);

		_totalAllocated -= LevelSize(level);
		_allocationsCount--;
	}
}

void BuddyAllocator::Deallocate(void* address)
{
	if (address) {
		ASSERT(false);
	}
}


}*/