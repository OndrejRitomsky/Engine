#include "HashMapBase.h"

#include "Core/Memory/Pointer.h"
#include "Core/Utility.h"

namespace core {

	static const u32 INTERNAL_INVALID_INDEX = U32MAX;


	namespace hash_map_base {
		struct FindResult {
			u32 modHashIndex;
			u32 prevDataIndex;
			u32 dataIndex;
		};

		FindResult FindInternal(const HashMapBase* base, Hash hash);

		//---------------------------------------------------------------------------
		FindResult FindInternal(const HashMapBase* base, Hash hash) {
			FindResult res;
			res.modHashIndex = INTERNAL_INVALID_INDEX;
			res.prevDataIndex = INTERNAL_INVALID_INDEX;
			res.dataIndex = INTERNAL_INVALID_INDEX;

			if (!base->capacity)
				return res;

			res.modHashIndex = hash % base->capacity;
			res.dataIndex = base->modHashToDataIndex[res.modHashIndex];

			u32 collisionCount = 0;
			while (res.dataIndex != INTERNAL_INVALID_INDEX) {
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
		bool Add(HashMapBase* base, Hash hash, u32 &outIndex) {
			FindResult res = hash_map_base::FindInternal(base, hash);

			if (res.dataIndex != INTERNAL_INVALID_INDEX) {
				outIndex = res.dataIndex;
				return false;
			}

			ASSERT(base->count < base->capacity);

			u32 dataInd = base->count;
			base->keys[dataInd] = hash;
			base->nexts[dataInd] = INTERNAL_INVALID_INDEX;

			if (res.prevDataIndex != INTERNAL_INVALID_INDEX)
				base->nexts[res.prevDataIndex] = dataInd;
			else 
				base->modHashToDataIndex[res.modHashIndex] = dataInd;

			outIndex = dataInd;
			return true;
		}

		//---------------------------------------------------------------------------
		// If the value isnt last, swap is made on keys and nexts
		//   not on VALUES! It has to be made and count has to be decreased
		bool Remove(const HashMapBase* base, Hash hash, u32& outIndex) {
			FindResult res = FindInternal(base, hash);
			if (res.dataIndex == INTERNAL_INVALID_INDEX)
				return false;

			outIndex = res.dataIndex;

			if (res.prevDataIndex != INTERNAL_INVALID_INDEX)
				base->nexts[res.prevDataIndex] = base->nexts[res.dataIndex];
			else
				base->modHashToDataIndex[res.modHashIndex] = base->nexts[res.dataIndex];

			base->keys[res.dataIndex] = base->keys[base->count - 1];
			base->nexts[res.dataIndex] = base->nexts[base->count - 1];

			outIndex = res.dataIndex;
			if (res.dataIndex == base->count - 1) // Is last, swap wont be made
				return true;

			FindResult resLast = FindInternal(base, base->keys[base->count - 1]);
			if (resLast.prevDataIndex != INTERNAL_INVALID_INDEX)
				base->nexts[resLast.prevDataIndex] = res.dataIndex;
			else
				base->modHashToDataIndex[resLast.modHashIndex] = res.dataIndex;

			return true;
		}

		//-------------------------------------------------------------------------
		void* Find(const HashMapBase* base, Hash hash, u32 elementSize) {
			FindResult res = FindInternal(base, hash);
			return res.dataIndex != INTERNAL_INVALID_INDEX ? static_cast<char*>(base->values) + elementSize * res.dataIndex : nullptr;
		}

		//-------------------------------------------------------------------------
		void ClearModHashes(const HashMapBase* base) {
			for (u32 i = 0; i < base->capacity; ++i)
				base->modHashToDataIndex[i] = INTERNAL_INVALID_INDEX;
		}

		//-------------------------------------------------------------------------
		HashMapBase Create(void* data, u32 capacity, u32 valueAlignment) {
			HashMapBase base;
			using namespace mem;
			base.modHashToDataIndex = static_cast<u32*>(Align(data, alignof(u32)));
			base.nexts = static_cast<u32*>(Align(base.modHashToDataIndex + capacity, alignof(u32)));
			base.keys = static_cast<Hash*>(Align(base.nexts + capacity, alignof(Hash)));
			base.values = Align(base.keys + capacity, valueAlignment);

			ASSERT((u64) (base.modHashToDataIndex + capacity) <= (u64) base.nexts);
			ASSERT((u64) (base.nexts + capacity) <= (u64) base.keys);
			ASSERT((u64) (base.keys + capacity) <= (u64) base.values);

			base.capacity = capacity;
			base._data = data;
			base.count = 0;

			ClearModHashes(&base);
			return base;
		}

		//-------------------------------------------------------------------------
		void CopyArrays(HashMapBase* dest, HashMapBase* source) {
			// Pointers are not copied
			if (dest->_data == source->_data) {
				memmove(dest->modHashToDataIndex, source->modHashToDataIndex, source->count * sizeof(u32));
				memmove(dest->nexts, source->nexts, source->count * sizeof(u32));
				memmove(dest->keys, source->keys, source->count * sizeof(Hash));
			}
			else {
				memcpy(dest->modHashToDataIndex, source->modHashToDataIndex, source->count * sizeof(u32));
				memcpy(dest->nexts, source->nexts, source->count * sizeof(u32));
				memcpy(dest->keys, source->keys, source->count * sizeof(Hash));
			}
			dest->count = source->count;
			dest->capacity = source->capacity;
		}

		//-------------------------------------------------------------------------
		void MoveyArrays(HashMapBase* dest, HashMapBase& source) {
			dest->modHashToDataIndex = source.modHashToDataIndex;
			dest->nexts = source.nexts;
			dest->keys = source.keys;
			dest->values = source.values;
			dest->_data = source._data;

			dest->count = source.count;
			dest->capacity = source.capacity;

			source.modHashToDataIndex = nullptr;
			source.nexts = nullptr;
			source.keys = nullptr;
			source.values = nullptr;
			source._data = nullptr;

			source.count = 0;
			source.capacity = 0;
		}
	}


}