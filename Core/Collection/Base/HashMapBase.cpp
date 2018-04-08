#include "HashMapBase.h"

#include "Core/Common/Pointer.h"
#include "Core/Algorithm/Memory.h"
#include "Core/Common/Utility.h"

namespace core {

	namespace hash_map_base {

		struct FindResult {
			u32 index;
			u32 prevDataIndex;
			u32 dataIndex;
		};

		//-------------------------------------------------------------------------
		void Init(HashMapBase* base, void* data, u32 capacity) {
			using namespace mem;
			base->indices = static_cast<u32*>(Align(data, alignof(u32)));
			base->nexts = static_cast<u32*>(Align(base->indices + capacity, alignof(u32)));
			base->keys = static_cast<h64*>(Align(base->nexts + capacity, alignof(h64)));

			ASSERT((u64) (base->indices + capacity) <= (u64) base->nexts);
			ASSERT((u64) (base->nexts + capacity) <= (u64) base->keys);

			base->capacity = capacity;
			base->count = 0;

			for (u32 i = 0; i < base->capacity; ++i)
				base->indices[i] = HashMapBase::INVALID_INDEX;
		}

		//---------------------------------------------------------------------------
		FindResult FindInternal(const HashMapBase* base, h64 hash) {
			FindResult res;
			res.index = HashMapBase::INVALID_INDEX;
			res.prevDataIndex = HashMapBase::INVALID_INDEX;
			res.dataIndex = HashMapBase::INVALID_INDEX;

			if (!base->capacity)
				return res;

			res.index = hash % base->capacity;
			res.dataIndex = base->indices[res.index];

			u32 collisionCount = 0;
			while (res.dataIndex != HashMapBase::INVALID_INDEX) {
				collisionCount++;
				// Just check for now
				ASSERT2(collisionCount < 10, "Keys are colliding a lot (slows performance), try different hash");

				if (base->keys[res.dataIndex] == hash)
					return res;

				res.prevDataIndex = res.dataIndex;
				res.dataIndex = base->nexts[res.dataIndex];
			}

			return res;
		}

		//---------------------------------------------------------------------------
		u32 Add(HashMapBase* base, h64 hash) {
			ASSERT(base->count < base->capacity);

			FindResult res = hash_map_base::FindInternal(base, hash);

			if (res.dataIndex != HashMapBase::INVALID_INDEX)
				return res.dataIndex;

			u32 dataInd = base->count;
			base->keys[dataInd] = hash;
			base->nexts[dataInd] = HashMapBase::INVALID_INDEX;

			if (res.prevDataIndex != HashMapBase::INVALID_INDEX) {
				base->nexts[res.prevDataIndex] = dataInd;
			}
			else {
				base->indices[res.index] = dataInd;
			}

			++base->count;

			return dataInd;
		}

		//---------------------------------------------------------------------------
		u32 SwapRemove(HashMapBase* base, h64 hash) {
			FindResult res = FindInternal(base, hash);
			if (res.dataIndex == HashMapBase::INVALID_INDEX)
				return HashMapBase::INVALID_INDEX;

			if (res.prevDataIndex != HashMapBase::INVALID_INDEX)
				base->nexts[res.prevDataIndex] = base->nexts[res.dataIndex];
			else
				base->indices[res.index] = base->nexts[res.dataIndex];

			--base->count;

			base->keys[res.dataIndex] = base->keys[base->count];
			base->nexts[res.dataIndex] = base->nexts[base->count];

			if (res.dataIndex == base->count) // Is last, swap wont be made
				return res.dataIndex;

			FindResult resLast = FindInternal(base, base->keys[base->count]);
			if (resLast.prevDataIndex != HashMapBase::INVALID_INDEX)
				base->nexts[resLast.prevDataIndex] = res.dataIndex;
			else
				base->indices[resLast.index] = res.dataIndex;

			return res.dataIndex;
		}

		//-------------------------------------------------------------------------
		void Clear(HashMapBase* base) {
			base->indices = nullptr;
			base->nexts = nullptr;
			base->keys = nullptr;

			base->count = 0;
		}

		//-------------------------------------------------------------------------
		bool IsFull(HashMapBase* base) {
			const f32 FULL_RATIO = 0.6f;
			return base->count >= static_cast<u32>(base->capacity * FULL_RATIO);
		}

		//-------------------------------------------------------------------------
		u32 Find(const HashMapBase* base, h64 hash) {
			return FindInternal(base, hash).dataIndex;
		}

		//-------------------------------------------------------------------------
		u32 SizeRequiredForCapacity(u32 capacity) {
			u32 needed = capacity * (2 * sizeof(u32) + sizeof(h64));
			return needed + 2 * alignof(u32) + alignof(h64);
		}

		//-------------------------------------------------------------------------
		void Copy(HashMapBase* dest, HashMapBase* source) {
			if (dest->indices != source->indices) {
				Memcpy(dest->indices, source->indices, source->count * sizeof(u32));
				Memcpy(dest->nexts, source->nexts, source->count * sizeof(u32));
				Memcpy(dest->keys, source->keys, source->count * sizeof(h64));
			}
			dest->count = source->count;
			dest->capacity = source->capacity;
		}

		//-------------------------------------------------------------------------
		void Move(HashMapBase* dest, HashMapBase&& source) {
			dest->indices = source.indices;
			dest->nexts = source.nexts;
			dest->keys = source.keys;

			dest->count = source.count;
			dest->capacity = source.capacity;

			source = {0};
		}
	}


}