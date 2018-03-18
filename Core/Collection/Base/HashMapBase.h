#pragma once

#include "Core/Types.h"
#include "Core/CoreTypes.h"

namespace core {

	// !!! Maybe you are looking for HashMap !!!



	// !!! Dont use this, if you dont understand the implementation
	// Its helpers for HashMap implementation (so template methods arent big and can be inlined)
	// Most of the functions leave the state invalid, operations on values (typed) must be done after !!!

	struct HashMapBase {
		u32 count;
		u32 capacity;

		void* _data;

		u32* modHashToDataIndex; // extra indirection so data can be contiguous

		u32* nexts;
		core::Hash* keys;
		void* values;
	};

	namespace hash_map_base {
		// Creates hashmapbase and sets pointers correctly taking value alignment into account
		HashMapBase Create(void* data, u32 capacity, u32 valueAlignment);

		// Adds data to arrays, !! except Values one (have to be done outside (with custruction))
		// count is not increased
		bool Add(HashMapBase* base, Hash hash, u32 &outIndex);

		//---------------------------------------------------------------------------
		// Removes data from arrays, except values, by swapping with last (if the element wasnt last)
		// !! values have to be swapped outside (with construction / deconstruction, move)
		// count is not decreased
		bool Remove(const HashMapBase* base, Hash hash, u32& outIndex);

		// Returns nullptr if not found or exact aligned addresse if found
		void* Find(const HashMapBase* base, Hash hash, u32 elementSize);

		void CopyArrays(HashMapBase* dest, HashMapBase* source);

		void MoveyArrays(HashMapBase* dest, HashMapBase& source); 
	}
}