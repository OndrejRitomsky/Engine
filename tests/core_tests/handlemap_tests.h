#pragma once

#include "../test.h"

#include <core/allocator/allocators.h>
#include <core/collection/handlemap.h>
#include <core/collection/handlemap.inl>


#define DEFINE_ALLOCATOR() \
	core::HeapAllocator heapAll; \
	core::IAllocator* allocator = HeapAllocatorInit(&heapAll); 

#define DEFINE_HANDLEMAP(type, name) \
	core::HandleMap<type> name; \
	name.Init(allocator);



TEST(HandleMapConstructorSizeTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);
		TEST_EQ(map.Count(), 0, "Size should be 0 after construction");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapConstructorCapacityTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);
		TEST_EQ(map.Capacity(), 0, "Capacity should be 0 after construction");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapAddTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);

		map.Add(3);
		map.Add(5);
		map.Add(7);

		TEST_EQ(map.Count(), 3, "Size should match amount of inserted elements");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapGetTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);

		int v = 3;
		core::Handle handle = map.Add(v);

		core::Handle h;
		h = handle;

		int value = map.Get(handle);

		TEST_EQ(value, v, "Should contain added element");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapRemoveTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);

		int v = 13;
		core::Handle handle = map.Add(v);

		map.Remove(handle);
		TEST_EQ(map.Count(), 0, "Size should match amount of inserted elements");
	}
	AllocatorDeinit(allocator);
}
TEST(HandleMapReserveTest1) {
	DEFINE_ALLOCATOR(); 
	{
		DEFINE_HANDLEMAP(int, map);
		int n = 45;
		map.Reserve(n);

		TEST_EQ(map.Capacity(), n, "Reserve should allocate propersize");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapReserveTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);
		for (int i = 0; i < 10; ++i) {
			map.Add(i);
		}

		int cap = map.Capacity();
		map.Reserve(cap - 2);

		TEST_EQ(map.Capacity(), cap, "Reserve shouldnt change capacity if its lower than current");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapBeginEndTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);
		for (int i = 0; i < 8; ++i) {
			map.Add(i);
		}

		long long int dis = map.end() - map.begin();

		TEST_EQ(map.Count(), dis, "Begin end distance should be same as size");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapCBeginCEndTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);
		for (int i = 0; i < 8; ++i) {
			map.Add(i);
		}

		// doesnt actualy call const version :)
		const int* b = map.begin();
		const int* e = map.end();
		long long int dis = e - b;

		TEST_EQ(map.Count(), dis, "Cbegin cend distance should be same as size");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapMultipleTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);

		const int N = 10;
		core::Handle handles[N];
		bool containsCheck[N];
		int valuesCheck[N];

		for (int i = 0; i < N; ++i) {
			valuesCheck[i] = i * 2 + 8;
			handles[i] = map.Add(valuesCheck[i]);
			containsCheck[i] = true;
		}

		const int R = 2;
		int removeIndexes[R] = {1,5};
		for (int i = 0; i < R; ++i) {
			int k = removeIndexes[i];
			containsCheck[k] = false;
			map.Remove(handles[k]);
			valuesCheck[k] = -1;
		}

		bool contains[N];
		int values[N];
		for (int i = 0; i < N; ++i) {
			if (i == 9) {
				int a = 3;
			}
			contains[i] = map.IsValid(handles[i]);
			values[i] = contains[i] ? map.Get(handles[i]) : -1;
		}

		TEST_ARRAY_EQ(containsCheck, contains, N, "Elements added should have valid handles");
		TEST_ARRAY_EQ(values, valuesCheck, N, "Elements added should match the values inside");
	}
	AllocatorDeinit(allocator);
}

TEST(HandleMapMultipleTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_HANDLEMAP(int, map);

		const int N = 10;
		core::Handle handles[2 * N];
		bool containsCheck[2 * N];
		int valuesCheck[2 * N];

		for (int i = 0; i < N; ++i) {
			valuesCheck[i] = i * 2;
			handles[i] = map.Add(valuesCheck[i]);
			containsCheck[i] = true;
		}

		const int R = 2;
		int removeIndexes[R] = {2, 8};
		for (int i = 0; i < R; ++i) {
			int k = removeIndexes[i];
			containsCheck[k] = false;
			map.Remove(handles[k]);
			valuesCheck[k] = -1;
		}

		for (int i = N; i < 2 * N; ++i) {
			valuesCheck[i] = i * 3;
			handles[i] = map.Add(valuesCheck[i]);
			containsCheck[i] = true;
		}

		bool contains[2 * N];
		int values[2 * N];
		for (int i = 0; i < 2 * N; ++i) {
			contains[i] = map.IsValid(handles[i]);
			values[i] = contains[i] ? map.Get(handles[i]) : -1;
		}

		TEST_ARRAY_EQ(values, valuesCheck, 2 * N, "Elements added should match the values inside");
	}
	AllocatorDeinit(allocator);
}

#undef DEFINE_ALLOCATOR
#undef DEFINE_HANDLEMAP

