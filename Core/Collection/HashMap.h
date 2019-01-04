#pragma once

#include "base/hashmapbase.h"
#include "../common/types.h"

namespace core {
	struct IAllocator;

	/**
	* HashMap<Type> : hashmap with in separate buffer linked list collision resolution
	* Keys have to be unique, HashMap does not value checks on equality
	* Lookup is with extra indirection
	* Data are contiguous, iterating is without indirection
	* HashMap is not initilized in constructor or init, first Add will reserve UNINITIALIZED_PUSH_INIT_CAPACITY
	* There is no limit to internal index collision (making it O(n) in worst case == colliding with n / k elements (but will assert on > 10))
	*/

	// @TODO iterator find -> iterator remove ??
	// @TODO equality has to be checked (need for interned strings)

	template<typename Value>
	class HashMap {
	private:
		static const u32 PUSH_INIT_CAPACITY = 17;

	public:
		struct ConstKeyValueIterator;
		struct KeyValueIterator;

	public:
		HashMap();
		HashMap(const HashMap& oth);
		HashMap(HashMap&& oth);
		~HashMap();

		HashMap& operator=(const HashMap& rhs);
		HashMap& operator=(HashMap&& rhs);

		void Init(IAllocator* allocator);

		Value* Find(h64 hash);
		const Value* Find(h64 hash) const;

		Value* begin();
		Value* end();

		const Value* begin() const;
		const Value* end() const;

		const h64* beginKey() const;
		const h64* endKey() const;

		// Key is always const!
		KeyValueIterator Iterator();
		const ConstKeyValueIterator CIterator() const;

		u32 Count() const;

		bool Add(h64 hash, const Value& value);
		bool Add(h64 hash, Value&& value);

		// Remove does swap on values and keys 
		// iterating - removing decrease count, don't increase index
		bool SwapRemove(h64 hash);

		void Clear();

	private:
		void Allocate(u32 capacity);

		void IncreaseCapacity();

	private:
		HashMapBase _base;
		Value* _values;
		IAllocator* _allocator;
	};

	template<typename Value>
	struct HashMap<Value>::KeyValueIterator {
		u32 count;
		const h64* keys;
		Value* values;
	};

	template<typename Value>
	struct HashMap<Value>::ConstKeyValueIterator {
		u32 count;
		const h64* keys;
		const Value* values;
	};
}


