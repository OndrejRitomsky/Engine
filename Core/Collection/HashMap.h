#pragma once

#include "Core/Types.h"
#include "Core/CoreTypes.h"
#include "Core/Collection/Base/HashMapBase.h"

namespace core {
	class IAllocator;

	/**
	* HashMap<Type> : hashmap with inplace linked list collision resolution implementation
	* Keys have to be unique, HashMap does not extra checks on equality
	* Lookup is with extra indirection
	* Data are contiguous, iterating is without indirection
	* HashMap is not initilized in constructor or init, first Add will reserve UNINITIALIZED_PUSH_INIT_CAPACITY
	* HashMap will double its size, if is its size reached treshold = FULL_RATIO * capacity;
	* There is no limit to internal index collision (making it O(n) in worst case == colliding with n / k elements (but will assert on > 5))
	*/

	// @TODO iterator find -> iterator remove

	template<typename Value>
	class HashMap {
	private:
		static const u32 PUSH_INIT_CAPACITY = 17;

	public:
		class ConstKeyValueIterator;
		friend class ConstKeyValueIterator;

	public:
		HashMap();
		HashMap(const HashMap& oth);
		HashMap(HashMap&& oth);
		~HashMap();

		HashMap& operator=(const HashMap& rhs);
		HashMap& operator=(HashMap&& rhs);

		void Init(IAllocator* allocator);

		Value* Find(Hash hash);
		const Value* Find(Hash hash) const;

		Value* begin();
		Value* end();

		const Value* cbegin() const;
		const Value* cend() const;

		const Hash* cKeyBegin() const;
		const Hash* cKeyEnd() const;

		ConstKeyValueIterator cKeyValueBegin() const;
		ConstKeyValueIterator cKeyValueEnd() const;

		u32 Count() const;

		bool Add(Hash hash, const Value& value);
		bool Add(Hash hash, Value&& value);

		bool Remove(Hash hash);

		void Clear();

	private:
		bool InternalAdd(Hash hash, u32& outIndex);

		HashMapBase Allocate(u32 capacity);

		void Rehash(u32 capacity);

	private:
		HashMapBase _base;
		IAllocator* _allocator;
	};
}


