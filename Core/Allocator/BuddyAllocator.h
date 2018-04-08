#pragma once

#include "Core/Allocator/IAllocator.h"
#include "Core/Common/Types.h"
/*
namespace core {

	class BuddyAllocator : public IAllocator {
	private:
		static const u32 MAX_LEVELS = 30;

	public:
		BuddyAllocator(u32 leafSize, u32 levels, u32 dataAllignment);
		~BuddyAllocator();

		// everything ll be alligned to leaf size;
		virtual void* Allocate(u64 size, u64 allignment) override;

		virtual void Deallocate(void* address) override; // doesnt support

		virtual void Deallocate(void* address, u64 size) override;

	private:

		void CleanAndMerge(u32 level, void* data);

		void* FindBlock(u32 level);
		u32 FindLevel(u32 size);

		u32 IndexInLevel(void* pointer, u32 level);

		inline u32 LevelSize(u32 level);

		inline u32 GlobalIndex(u32 level, u32 indexInLevel);

	private:
		u32 _levels;

		u32 _leafSize;

		u32 _bufferSize;

		u32 _totalAllocated;
		u32 _allocationsCount;

		void* _freeLists[MAX_LEVELS];

		// todo solve allocation
		u32* _occupiedBits;

		void* _start;
	};
	// ---------------------------------------------------------------------------
	// ---------------------------------------------------------------------------


	// ------------------------------------------------------------------- INLINES

	inline u32 BuddyAllocator::LevelSize(u32 level) { return (_bufferSize / (1 << level)); }
	inline u32 BuddyAllocator::GlobalIndex(u32 level, u32 indexInLevel) { return (1 << level) + indexInLevel - 1; }

}

*/