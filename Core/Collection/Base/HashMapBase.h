#pragma once

#include "Core/Common/Types.h"
#include "Core/Common/TypeLimits.h"

namespace core {

	struct HashMapBase {
		enum { INVALID_INDEX = U32MAX };

		u32 count;
		u32 capacity;

		u32* indices; // extra indirection so data can be contiguous
		u32* nexts;
		h64* keys;
	};

	namespace hash_map_base {
		void Init(HashMapBase* base, void* data, u32 capacity);

		u32 Find(const HashMapBase* base, h64 hash);

		u32 Add(HashMapBase* base, h64 hash);

		u32 SwapRemove(HashMapBase* base, h64 hash);

		void Clear(HashMapBase* base);

		bool IsFull(HashMapBase* base);

		void Copy(HashMapBase* dest, HashMapBase* source);

		void Move(HashMapBase* dest, HashMapBase&& source); 

		u32 SizeRequiredForCapacity(u32 capacity);
	}
}