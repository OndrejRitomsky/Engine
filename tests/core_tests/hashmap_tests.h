#pragma once

#include "../test.h"

#include <core/allocator/allocators.h>
#include <core/algorithm/hash.h>
#include <core/collection/hashmap.h>
#include <core/collection/hashmap.inl>


#define DEFINE_ALLOCATOR() \
	core::HeapAllocator heapAll; \
	core::IAllocator* allocator = HeapAllocatorInit(&heapAll); 

#define DEFINE_HASHMAP(type, name) \
	core::HashMap<type> name; \
	name.Init(allocator);


TEST(HashMapConstructorSizeTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);

		TEST_EQ(map.Count(), 0, "Size should be 0 after construction");
	}
	AllocatorDeinit(allocator);
}
TEST(HashMapAddTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);

		int a = 32;
		map.Add(core::Hash(&a, sizeof(int)), a);

		int b = 33;
		map.Add(core::Hash(&b, sizeof(int)), b);


		TEST_EQ(map.Count(), 2, "Size should match amount of inserted elements");
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapFindTest1p) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);

		int e = 32;
		h64 hash = core::Hash(&e, sizeof(int));
		map.Add(hash, e);


		int* element = map.Find(hash);

		TEST_EQ(*element, e, "Find should return pointer to the same value as was added in");
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapFindTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);

		int a = 99;
		h64 hash = core::Hash(&a, sizeof(int));


		int* element = map.Find(hash);

		TEST_EQ(element, nullptr, "Find should return nullptr, element is not inside");
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapRemoveTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);

		int a = 99;
		h64 hash = core::Hash(&a, sizeof(int));
		map.Add(hash, a);
		map.SwapRemove(hash);

		TEST_EQ(map.Count(), 0, "Size should match amount of inserted elements");
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapRemoveTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);

		int a = 99;
		h64 hash = core::Hash(&a, sizeof(int));
		map.SwapRemove(hash);

		TEST_EQ(map.Count(), 0, "Size should match amount of inserted elements");
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapIterator) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);
		for (int i = 0; i < 8; ++i) {
			h64 hash = core::Hash(&i, sizeof(int));
			map.Add(hash, i);
		}

		long long int dis = map.end() - map.begin();
		TEST_EQ(map.Count(), dis, "Begin end distance should be same as size");
	}
	AllocatorDeinit(allocator);
}


TEST(HashMapConstIterator) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);
		for (int i = 0; i < 11; ++i) {
			h64 hash = core::Hash(&i, sizeof(int));
			map.Add(hash, i);
		}

		long long int dis = map.end() - map.begin();
		TEST_EQ(map.Count(), dis, "Cbegin cend distance should be same as size");
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapConstKeyValueIterator) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(int, map);

		const int n = 2;
		int values[n];
		h64 hashes[n];

		for (int i = 0; i < n; ++i) {
			h64 hash = core::Hash(&i, sizeof(int));
			map.Add(hash, i);
			values[i] = i;
			hashes[i] = hash;
		}

		auto it = map.CIterator();
		for (unsigned i = 0; i < it.count; ++i) {
			TEST_EQ(it.keys[i], hashes[i], "KeyValue iterator key equality");
			TEST_EQ(it.values[i], values[i], "KeyValue iterator value equality");
		}
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapMultipleTest1) {
	struct Str {
		int a;
		bool operator==(const Str& rhs) const { return a == rhs.a; }
	};

	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(Str, map);

		const int N = 10;
		h64 hashes[N];
		bool containsCheck[N];


		for (int i = 0; i < N; ++i) {
			Str s = {i * i + 9};
			hashes[i] = core::Hash(&s, sizeof(Str));
			map.Add(hashes[i], s);
			containsCheck[i] = true;
		}

		const int R = 2;
		int removeIndexes[R] = {1,7};
		for (int i = 0; i < R; ++i) {
			containsCheck[removeIndexes[i]] = false;
			map.SwapRemove(hashes[removeIndexes[i]]);
		}

		bool contains[N];
		for (int i = 0; i < N; ++i) {
			contains[i] = map.Find(hashes[i]) != nullptr;
		}

		TEST_ARRAY_EQ(containsCheck, contains, N, "Elements added should be inside and elements removed should not");
	}
	AllocatorDeinit(allocator);
}

TEST(HashMapMultipleTest2) {
	struct Str {
		float a;
		bool operator==(const Str& rhs) const { return a == rhs.a; }
	};

	DEFINE_ALLOCATOR();
	{
		DEFINE_HASHMAP(Str, map);

		const int N = 40;
		h64 hashes[N];
		bool containsCheck[N];

		for (int i = 0; i < N; ++i) {
			Str s = {i + 9.f};
			hashes[i] = core::Hash(&s, sizeof(Str));
			map.Add(hashes[i], s);
			containsCheck[i] = true;
		}

		const int R = 4;
		int removeIndexes[R] = {4,14,32,38};
		for (int i = 0; i < R; ++i) {
			containsCheck[removeIndexes[i]] = false;
			map.SwapRemove(hashes[removeIndexes[i]]);
		}

		bool contains[N];
		for (int i = 0; i < N; ++i) {
			contains[i] = map.Find(hashes[i]) != nullptr;
		}

		TEST_ARRAY_EQ(containsCheck, contains, N, "Elements added should be inside and elements removed should not");
	}
	AllocatorDeinit(allocator);
}
