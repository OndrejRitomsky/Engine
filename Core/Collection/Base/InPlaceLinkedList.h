#pragma once

#include "../../common/types.h"

namespace core {
	// Static Array with holes
	// Uses free list -> element size has to be >= sizeof(u32)
	// No error handling is done, only asserts
	// Clear makes free list invalid
	// Array shouldnt be reallocated when it has holes,
	//   currently there is no function to query this, but its not needed in lookuparray

	struct InPlaceLinkedList {
		u32 freeFirst;
		u32 countWithHoles;
		void* data;
	};

	namespace in_place_linked_list {
		void Init(InPlaceLinkedList* base, void* data);

		void Clear(InPlaceLinkedList* base);

		void Remove(InPlaceLinkedList* base, u32 index, u32 elementSize);

		u32 Add(InPlaceLinkedList* base, u32 elementSize);
	}
}