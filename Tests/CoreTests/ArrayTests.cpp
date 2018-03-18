#include "ArrayTests.h"

#include <TestSuite/Test.h>

#include <Core/CoreAssert.h>
#include <TestSuite/throwAssert.h>

#include <Core/Collection/Array.h>
#include <Core/Collection/Array.inl>

#include <Core/Allocator/HeapAllocator.h>

namespace array_tests
{
	//---------------------------------------------------------------------------
	void ConstructorSizeTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		TestEqual(testGroup, arr.Count(), 0, "Size should be 0, after contruction");
	}

	//---------------------------------------------------------------------------
	void ConstructorCapacityTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		TestEqual(testGroup, arr.Capacity(), 0, "Capacity should be 0, after contruction");
	}

	//---------------------------------------------------------------------------
	void CopyConstructorTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		int n = 12;
		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2(arr);

		TestArrayEqual(testGroup, arr, arr2, n, "Values should be equal to copy constructed array");
		TestEqual(testGroup, arr.Count(), arr2.Count(), "Size should be equal to copy constructed array");
		TestEqual(testGroup, arr.Capacity(), arr2.Capacity(), "Capacity should be equal to copy constructed array");
	}

	//---------------------------------------------------------------------------
	void MoveConstructorTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		int n = 12;
		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2(std::move(arr));

		TestEqual(testGroup, arr.Count(), 0, "Size should be zero after move contructor");
		TestEqual(testGroup, arr.Capacity(), 0, "Capacity should be zero after move constructor");
	}

	//---------------------------------------------------------------------------
	void MoveConstructorTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		const int k = 6;
		int n[k] = {1, 2, 3, 4, 5, 6};
		for (int i = 0; i < k; ++i) {
			arr.Push(n[i]);
		}

		core::Array<int> arr2(std::move(arr));

		TestArrayEqual(testGroup, arr2, n, k, "Moved values shouldnt be different than original values");
	}

	//---------------------------------------------------------------------------
	void AssignmentOperatorTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		int n = 12;
		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2 = arr;
		TestArrayEqual(testGroup, arr, arr2, n, "Values should be equal to array created by assignment op");
		TestEqual(testGroup, arr.Count(), arr2.Count(), "Size should be equal to array created by assignment op");
		TestEqual(testGroup, arr.Capacity(), arr2.Capacity(), "Capacity should be equal to array created by assignment op");
	}

	//---------------------------------------------------------------------------
	void MoveAssignmentOperatorTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		int n = 12;
		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2 = std::move(arr);

		TestEqual(testGroup, arr.Count(), 0, "Size should be zero after move contructor");
		TestEqual(testGroup, arr.Capacity(), 0, "Capacity should be zero after move constructor");
	}

	//---------------------------------------------------------------------------
	void MoveAssignmentOperatorTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		const int k = 3;
		int n[k] = {1, 2, 3};
		for (int i = 0; i < k; ++i) {
			arr.Push(n[i]);
		}

		core::Array<int> arr2 = std::move(arr);

		TestArrayEqual(testGroup, arr2, n, k, "Moved values shouldnt be different than original values");
	}

	//---------------------------------------------------------------------------
	void PushTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		arr.Push(0);

		TestEqual(testGroup, arr.Count(), 1, "Size should be 1");
	}

	//---------------------------------------------------------------------------
	void PushTest2(test::TestGroup& testGroup) {
		struct TestStruct {
			int a;

			bool operator==(TestStruct& oth) {
				return a == oth.a;
			}
		};

		core::HeapAllocator allocator;
		core::Array<TestStruct> arr;
		arr.Init(&allocator);

		TestStruct structs[2];
		structs[0].a = 5;
		structs[1].a = 47;

		TestStruct struct1;
		struct1.a = 5;
		TestStruct struct2;
		struct2.a = 47;

		arr.Push(struct1);
		arr.Push(struct2);

		TestEqual(testGroup, arr.Count(), 2, "Size should be 2");
		TestArrayEqual(testGroup, arr, structs, 2, "Elements should match");
	}

	//---------------------------------------------------------------------------
	void PushMultipleTest1(test::TestGroup& testGroup) {
		struct TestStruct {
			int a;

			bool operator==(TestStruct& oth) {
				return a == oth.a;
			}
		};

		core::HeapAllocator allocator;
		core::Array<TestStruct> arr;
		arr.Init(&allocator);

		TestStruct structs[3];
		structs[0].a = 42;
		structs[1].a = 13;
		structs[2].a = 22;

		arr.PushValues(structs, 3);

		TestEqual(testGroup, (int) arr.Count(), 3, "Size should be 3");
		TestArrayEqual(testGroup, arr, structs, 3, "Elements should match");
	}

	//---------------------------------------------------------------------------
	void TopTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		arr.Push(0);

		int a = 5;
		arr.Push(a);
		int b = arr.Top();
		TestEqual(testGroup, a, b, "Top value should be the last pushed");
	}

	//---------------------------------------------------------------------------
	void PopTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		arr.Push(0);

		int b = 5;
		arr.Push(b);
		arr.Pop();
		TestEqual(testGroup, arr.Count(), 1, "Size should be 1");
	}

	//---------------------------------------------------------------------------
	void PopTest2(test::TestGroup& testGroup) {
		core::Array<int> arr;
		TestAssert(testGroup, arr.Pop(), "Should fire assert on invalid pop");
	}

	//---------------------------------------------------------------------------
	void PopTest3(test::TestGroup& testGroup) {
		int counter = 0;
		struct Str {
			int* counter;
			~Str() {
				if (counter)
					(*counter)++;
			}
		};

		core::HeapAllocator allocator;
		core::Array<Str> arr;
		arr.Init(&allocator);

		arr.Push(std::move(Str()));
		arr[0].counter = &counter;
		arr.Pop();

		TestEqual(testGroup, counter, 1, "Pop should properly destruct element");
	}

	//---------------------------------------------------------------------------
	void RemoveTest1(test::TestGroup& testGroup) {
		int counter = 0;
		struct Str {
			int* counter;
			~Str() {
				if (counter)
					(*counter)++;
			}
		};

		core::HeapAllocator allocator;
		core::Array<Str> arr;
		arr.Init(&allocator);

		arr.Push(std::move(Str()));
		arr[0].counter = &counter;
		arr.Remove(0);

		TestEqual(testGroup, counter, 1, "Remove should properly destruct element");
	}

	//---------------------------------------------------------------------------
	void BracketOperatorTest1(test::TestGroup& testGroup) {
		core::Array<int> arr;
		TestAssert(testGroup, arr[0], "Should fire assert on invalid index");
	}

	//---------------------------------------------------------------------------
	void BracketOperatorTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		int a = 48;
		arr.Push(a);
		TestEqual(testGroup, arr[0], a, "Indexed value should be same as value added");
	}

	//---------------------------------------------------------------------------
	void IsEmptyTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		arr.Push(3);
		arr.Pop();
		TestTrue(testGroup, arr.IsEmpty(), "Should be empty");
	}

	//---------------------------------------------------------------------------
	void CapacityTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		arr.Push(3);
		arr.Push(4);
		arr.Push(5);

		TestLowerEqual(testGroup, arr.Count(), arr.Capacity(), "Capacity should be <= array size");
	}

	//---------------------------------------------------------------------------
	void CompactTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		for (int i = 0; i < 47; ++i) {
			arr.Push(i);
		}
		arr.Compact();

		TestEqual(testGroup, arr.Count(), arr.Capacity(), "Size should equal its capacity after compact");
	}

	//---------------------------------------------------------------------------
	void ClearTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}
		arr.Clear();

		TestEqual(testGroup, arr.Count(), 0, "Size should equal its capacity after compact");
	}

	//---------------------------------------------------------------------------
	void ClearTest2(test::TestGroup& testGroup) {
		int counter = 0;
		struct Str {
			int* counter;
			~Str() {
				if (counter)
					(*counter)++;
			}
		};

		core::HeapAllocator allocator;
		core::Array<Str> arr;
		arr.Init(&allocator);
		int n = 4;
		for (int i = 0; i < n; ++i) {
			arr.Push(std::move(Str()));
			arr[i].counter = &counter;
		}
		arr.Clear();

		TestEqual(testGroup, counter, n, "Clear should properly destruct elements");
	}

	//---------------------------------------------------------------------------
	void FindTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		for (int i = 0; i < 12; ++i) {
			arr.Push(i * 2);
		}

		int r = (int) arr.Find(4);

		TestNotEqual(testGroup, r, core::Array<int>::INVALID_INDEX, "Find should not return invalid index");
		TestLower(testGroup, r, (int) arr.Count(), "Find index in size");
	}

	//---------------------------------------------------------------------------
	void FindTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);

		for (int i = 0; i < 5; ++i) {
			arr.Push(i * 9);
		}

		int r = arr.Find(9999);

		TestEqual(testGroup, r, core::Array<int>::INVALID_INDEX, "Find should return invalid index")
	}

	//---------------------------------------------------------------------------
	void ReserveTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		unsigned int n = 43;
		arr.Reserve(n);

		TestLowerEqual(testGroup, n, arr.Capacity(), "Reserve should allocate enough capacity");
	}

	//---------------------------------------------------------------------------
	void ReserveTest2(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		for (int i = 0; i < 10; ++i) {
			arr.Push(i);
		}

		int cap = arr.Capacity();
		arr.Reserve(cap - 2);

		TestEqual(testGroup, arr.Capacity(), cap, "Reserve shouldnt change capacity if its lower than current");
	}

	//---------------------------------------------------------------------------
	void BeginEndTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		for (int i = 0; i < 10; ++i) {
			arr.Push(i);
		}

		long long int dis = arr.end() - arr.begin();

		TestEqual(testGroup, arr.Count(), dis, "Begin end distance should be same as size");
	}

	//---------------------------------------------------------------------------
	void CBeginCEndTest1(test::TestGroup& testGroup) {
		core::HeapAllocator allocator;
		core::Array<int> arr;
		arr.Init(&allocator);
		for (int i = 0; i < 10; ++i) {
			arr.Push(i);
		}

		long long int dis = arr.cend() - arr.cbegin();

		TestEqual(testGroup, arr.Count(), dis, "Cbegin cend distance should be same as size");
	}
}
