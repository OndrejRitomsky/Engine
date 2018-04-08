#include "HandleMapTests.h"

#include <TestSuite/Test.h>

#include <Core/Allocator/HeapAllocator.h>

#include <Core/Collection/HandleMap.h>
#include <Core/Collection/HandleMap.inl>

namespace handleMap_tests {

	//---------------------------------------------------------------------------
	void ConstructorSizeTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());

		TestEqual(testGroup, map.Count(), 0, "Size should be 0 after construction");
	}

	//---------------------------------------------------------------------------
	void ConstructorCapacityTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());

		TestEqual(testGroup, map.Capacity(), 0, "Capacity should be 0 after construction");
	}

	//---------------------------------------------------------------------------
	void AddTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());

		map.Add(3);
		map.Add(5);
		map.Add(7);

		TestEqual(testGroup, map.Count(), 3, "Size should match amount of inserted elements");
	}

	//---------------------------------------------------------------------------
	void GetTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());

		int v = 3;
		core::Handle handle = map.Add(v);

		core::Handle h;
		h = handle;

		int value = map.Get(handle);

		TestEqual(testGroup, value, v, "Should contain added element");
	}

	//---------------------------------------------------------------------------
	void RemoveTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());

		int v = 13;
		core::Handle handle = map.Add(v);

		map.Remove(handle);
		TestEqual(testGroup, map.Count(), 0, "Size should match amount of inserted elements");
	}

	//---------------------------------------------------------------------------
	void ReserveTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());
		int n = 45;
		map.Reserve(n);

		TestEqual(testGroup, map.Capacity(), n, "Reserve should allocate propersize");
	}

	//---------------------------------------------------------------------------
	void ReserveTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());
		for (int i = 0; i < 10; ++i) {
			map.Add(i);
		}

		int cap = map.Capacity();
		map.Reserve(cap - 2);

		TestEqual(testGroup, map.Capacity(), cap, "Reserve shouldnt change capacity if its lower than current");
	}

	//---------------------------------------------------------------------------
	void BeginEndTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());
		for (int i = 0; i < 8; ++i) {
			map.Add(i);
		}

		long long int dis = map.end() - map.begin();

		TestEqual(testGroup, map.Count(), dis, "Begin end distance should be same as size");
	}

	//---------------------------------------------------------------------------
	void CBeginCEndTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());
		for (int i = 0; i < 8; ++i) {
			map.Add(i);
		}

		// doesnt actualy call const version :)
		const int* b = map.begin();
		const int* e = map.end();
		long long int dis = e - b;

		TestEqual(testGroup, map.Count(), dis, "Cbegin cend distance should be same as size");
	}

	//---------------------------------------------------------------------------
	void MultipleTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());

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

		TestArrayEqual(testGroup, containsCheck, contains, N, "Elements added should have valid handles");
		TestArrayEqual(testGroup, values, valuesCheck, N, "Elements added should match the values inside");
	}

	//---------------------------------------------------------------------------
	void MultipleTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HandleMap<int> map;
		map.Init(allocator.Allocator());

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

	//	TestArrayEqual(testGroup, containsCheck, contains, 2 * N, "Elements added should have valid handles");
		TestArrayEqual(testGroup, values, valuesCheck, 2 * N, "Elements added should match the values inside");
	}

}
