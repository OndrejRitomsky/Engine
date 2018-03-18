#pragma once

#include "Core/Types.h"

namespace core {
	// Static Array with holes
	// Uses free list -> element size has to be >= sizeof(u32)
	// No error handling is done, only asserts
	// Clear makes free list invalid
	// Array shouldnt be reallocated when it has holes, 
	//   currently there is no function to query this, but its not needed in lookuparray

	struct LookupArrayBase {
		u32 elementSize;
		u32 freeFirst;

		u32 size; // with holes
		u32 capacity;

		void* data;
	};

	namespace lookup_array_base {
		void Clear(LookupArrayBase* base);
		void Remove(LookupArrayBase* base, u32 index);

		void* Get(const LookupArrayBase* base, u32 index);

		u32 Add(LookupArrayBase* base);

		bool IsFull(const LookupArrayBase* base);
	}
}