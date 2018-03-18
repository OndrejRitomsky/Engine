#include "HashMapTests.h"

#include <TestSuite/Test.h>

#include <Core/Allocator/HeapAllocator.h>
#include <Core/Algorithm/Hash.h>
#include <Core/Collection/HashMap.h>
#include <Core/Collection/HashMap.inl>


namespace hashMap_tests {
	//---------------------------------------------------------------------------
	void ConstructorSizeTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HashMap<int> map;
		map.Init(&allocator);

		TestEqual(testGroup, map.Count(), 0, "Size should be 0 after construction");
	}

	//---------------------------------------------------------------------------
	void AddTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;	
		core::HashMap<int> map;
		map.Init(&allocator);

		int a = 32;
		map.Add(core::ToHash(&a, sizeof(int)), a);
		
		int b = 33;
		map.Add(core::ToHash(&b, sizeof(int)), b);


		TestEqual(testGroup, map.Count(), 2, "Size should match amount of inserted elements");
	}

	//---------------------------------------------------------------------------
	void FindTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HashMap<int> map;
		map.Init(&allocator);

		int e = 32;
		core::Hash hash = core::ToHash(&e, sizeof(int));
		map.Add(hash, e);


		int* element = map.Find(hash);
		
		TestEqual(testGroup, *element, e, "Find should return pointer to the same value as was added in");
	}

	//---------------------------------------------------------------------------
	void FindTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HashMap<int> map;
		map.Init(&allocator);

		int a = 99;
		core::Hash hash = core::ToHash(&a, sizeof(int));

		
		int* element = map.Find(hash);

		TestEqual(testGroup, element, nullptr, "Find should return nullptr, element is not inside");
	}
	
	//---------------------------------------------------------------------------
	void RemoveTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HashMap<int> map;
		map.Init(&allocator);

		int a = 99;
		core::Hash hash = core::ToHash(&a, sizeof(int));
		map.Add(hash, a);
		map.Remove(hash);

		TestEqual(testGroup, map.Count(), 0, "Size should match amount of inserted elements");
	}

	//---------------------------------------------------------------------------
	void RemoveTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HashMap<int> map;
		map.Init(&allocator);

		int a = 99;
		core::Hash hash = core::ToHash(&a, sizeof(int));
		map.Remove(hash);

		TestEqual(testGroup, map.Count(), 0, "Size should match amount of inserted elements");
	}
	//---------------------------------------------------------------------------

	void Iterator(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HashMap<int> map;
		map.Init(&allocator);
		for (int i = 0; i < 8; ++i) {
			core::Hash hash = core::ToHash(&i, sizeof(int));
			map.Add(hash, i);
		}

		long long int dis = map.end() - map.begin();
		TestEqual(testGroup, map.Count(), dis, "Begin end distance should be same as size");
  }

	//---------------------------------------------------------------------------
	void ConstIterator(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::HashMap<int> map;
		map.Init(&allocator);
		for (int i = 0; i < 11; ++i) {
			core::Hash hash = core::ToHash(&i, sizeof(int));
			map.Add(hash, i);
		}

		long long int dis = map.cend() - map.cbegin();
		TestEqual(testGroup, map.Count(), dis, "Cbegin cend distance should be same as size");
	}

  //---------------------------------------------------------------------------
  void ConstKeyValueIterator(test::TestGroup& testGroup) {
    core::HeapAllocator allocator;
    core::HashMap<int> map;
    map.Init(&allocator);

    const int n = 2;
    int values[n];
    core::Hash hashes[n];

    for (int i = 0; i < n; ++i) {
      core::Hash hash = core::ToHash(&i, sizeof(int));
      map.Add(hash, i);
      values[i] = i;
      hashes[i] = hash;
    }

    int i = 0;
    for (auto& it = map.cKeyValueBegin(); it != map.cKeyValueEnd(); ++it, i++) {
      TestEqual(testGroup, it.key, hashes[i], "KeyValue iterator key equality");
      TestEqual(testGroup, it.value, values[i], "KeyValue iterator value equality");
    }
  }



	//---------------------------------------------------------------------------
	void MultipleTest1(test::TestGroup& testGroup) {
		struct Str {
			int a;
			bool operator==(const Str& rhs) const { return a == rhs.a; }
		};

		core::HeapAllocator allocator;
		core::HashMap<Str> map;
		map.Init(&allocator);

		const int N = 10;
		core::Hash hashes[N];
		bool containsCheck[N];


		for (int i = 0; i < N; ++i) {
			Str s = {i * i + 9};
			hashes[i] = core::ToHash(&s, sizeof(Str));
			map.Add(hashes[i], s);
			containsCheck[i] = true;
		}

		const int R = 2;
		int removeIndexes[R] = {1,7};
		for (int i = 0; i < R; ++i) {
			containsCheck[removeIndexes[i]] = false;
			map.Remove(hashes[removeIndexes[i]]);
		}

		bool contains[N];
		for (int i = 0; i < N; ++i) {
			contains[i] = map.Find(hashes[i]) != nullptr;
		}

		TestArrayEqual(testGroup, containsCheck, contains, N, "Elements added should be inside and elements removed should not");
	}

	//---------------------------------------------------------------------------
	void MultipleTest2(test::TestGroup& testGroup) {
		struct Str {
			float a;
			bool operator==(const Str& rhs) const { return a == rhs.a; }
		};

		core::HeapAllocator allocator;
		core::HashMap<Str> map;
		map.Init(&allocator);

		const int N = 40;
		core::Hash hashes[N];
		bool containsCheck[N];

		for (int i = 0; i < N; ++i) {
			Str s = {i + 9.f};
			hashes[i] = core::ToHash(&s, sizeof(Str));
			map.Add(hashes[i], s);
			containsCheck[i] = true;
		}

		const int R = 4;
		int removeIndexes[R] = {4,14,32,38};
		for (int i = 0; i < R; ++i) {
			containsCheck[removeIndexes[i]] = false;
			map.Remove(hashes[removeIndexes[i]]);
		}

		bool contains[N];
		for (int i = 0; i < N; ++i) {
			contains[i] = map.Find(hashes[i]) != nullptr;
		}

		TestArrayEqual(testGroup, containsCheck, contains, N, "Elements added should be inside and elements removed should not");
	}
}