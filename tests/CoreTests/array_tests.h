#pragma once


#include "../test.h"

#include "core/collection/array.h"
#include "core/collection/array.inl"
#include "core/allocator/allocators.h"

#define DEFINE_ALLOCATOR() \
	core::HeapAllocator heapAll; \
	core::IAllocator* allocator = HeapAllocatorInit(&heapAll); 

#define DEFINE_ARRAY(type, name) \
	core::Array<type> name; \
	name.Init(allocator);


TEST(ArrayConstructorSizeTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		TEST_EQ(arr.Count(), 0, "Size should be 0, after contruction");
		AllocatorDeinit(allocator);
	}
}

TEST(ArrayConstructorCapacityTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		TEST_EQ(arr.Capacity(), 0, "Capacity should be 0, after contruction");
		AllocatorDeinit(allocator);
	}
}

TEST(ArrayCopyConstructorTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);

		unsigned int n = 12;
		for (unsigned int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2(arr);

		TEST_ARRAY_EQ(arr, arr2, n, "Values should be equal to copy constructed array");
		TEST_EQ(arr.Count(), arr2.Count(), "Size should be equal to copy constructed array");
		TEST_EQ(arr.Capacity(), arr2.Capacity(), "Capacity should be equal to copy constructed array");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayMoveConstructorTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		int n = 12;
		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2(core::move(arr));

		TEST_EQ(arr.Count(), 0, "Size should be zero after move contructor");
		TEST_EQ(arr.Capacity(), 0, "Capacity should be zero after move constructor");
	}
	AllocatorDeinit(allocator);
}
TEST(ArrayMoveConstructorTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		const int k = 6;
		int n[k] = {1, 2, 3, 4, 5, 6};
		for (int i = 0; i < k; ++i) {
			arr.Push(n[i]);
		}

		core::Array<int> arr2(core::move(arr));

		TEST_ARRAY_EQ(arr2, arr2, k, "Moved values shouldnt be different than original values");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayAssignmentOperatorTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		unsigned int n = 12;
		for (unsigned int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2 = arr;
		TEST_ARRAY_EQ(arr, arr2, n, "Values should be equal to array created by assignment op");
		TEST_EQ(arr.Count(), arr2.Count(), "Size should be equal to array created by assignment op");
		TEST_EQ(arr.Capacity(), arr2.Capacity(), "Capacity should be equal to array created by assignment op");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayMoveAssignmentOperatorTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		int n = 12;
		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}

		core::Array<int> arr2 = core::move(arr);

		TEST_EQ(arr.Count(), 0, "Size should be zero after move contructor");
		TEST_EQ(arr.Capacity(), 0, "Capacity should be zero after move constructor");
	}
	AllocatorDeinit(allocator);
}


TEST(ArrayMoveAssignmentOperatorTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		const int k = 3;
		int n[k] = {1, 2, 3};
		for (int i = 0; i < k; ++i) {
			arr.Push(n[i]);
		}

		core::Array<int> arr2 = core::move(arr);

		TEST_ARRAY_EQ(arr2, n, k, "Moved values shouldnt be different than original values");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayPushTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		arr.Push(0);
		TEST_EQ(arr.Count(), 1, "Size should be 1");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayPushTest2) {
	struct TestStruct {
		int a;

		bool operator==(TestStruct& oth) {
			return a == oth.a;
		}
	};

	DEFINE_ALLOCATOR(); 
	{
		DEFINE_ARRAY(TestStruct, arr);

		TestStruct structs[2];
		structs[0].a = 5;
		structs[1].a = 47;

		TestStruct struct1;
		struct1.a = 5;
		TestStruct struct2;
		struct2.a = 47;

		arr.Push(struct1);
		arr.Push(struct2);

		TEST_EQ(arr.Count(), 2, "Size should be 2");
		TEST_ARRAY_EQ(arr, structs, 2, "Elements should match");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayPushMultipleTest1) {
	struct TestStruct {
		int a;

		bool operator==(TestStruct& oth) {
			return a == oth.a;
		}
	};

	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(TestStruct, arr);

		TestStruct structs[3];
		structs[0].a = 42;
		structs[1].a = 13;
		structs[2].a = 22;

		arr.PushValues(structs, 3);

		TEST_EQ((int) arr.Count(), 3, "Size should be 3");
		TEST_ARRAY_EQ(arr, structs, 3, "Elements should match");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayTopTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);

		arr.Push(0);

		int a = 5;
		arr.Push(a);
		int b = arr.Top();
		TEST_EQ(a, b, "Top value should be the last pushed");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayPopTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);

		arr.Push(0);

		int b = 5;
		arr.Push(b);
		arr.Pop();
		TEST_EQ(arr.Count(), 1, "Size should be 1");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayPopTest3) {
	int counter = 0;
	struct Str {
		int* counter;
		~Str() {
			if (counter)
				(*counter)++;
		}
	};

	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(Str, arr);

		arr.Push(core::move(Str()));
		arr[0].counter = &counter;
		arr.Pop();

		TEST_EQ(counter, 1, "Pop should properly destruct element");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayRemoveTest1) {
	int counter = 0;
	struct Str {
		int* counter;
		~Str() {
			if (counter)
				(*counter)++;
		}
	};

	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(Str, arr);

		arr.Push(core::move(Str()));
		arr[0].counter = &counter;
		arr.Remove(0);

		TEST_EQ(counter, 1, "Remove should properly destruct element");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayBracketOperatorTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		int a = 48;
		arr.Push(a);
		TEST_EQ(arr[0], a, "Indexed value should be same as value added");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayIsEmptyTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		arr.Push(3);
		arr.Pop();
		TEST_TRUE(arr.IsEmpty(), "Should be empty");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayCapacityTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		arr.Push(3);
		arr.Push(4);
		arr.Push(5);

		TEST_LE(arr.Count(), arr.Capacity(), "Capacity should be <= array size");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayCompactTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);

		for (int i = 0; i < 47; ++i) {
			arr.Push(i);
		}
		arr.Compact();

		TEST_EQ(arr.Count(), arr.Capacity(), "Size should equal its capacity after compact");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayClearTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);

		for (int i = 0; i < 12; ++i) {
			arr.Push(i);
		}
		arr.Clear();

		TEST_EQ(arr.Count(), 0, "Size should equal its capacity after compact");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayClearTest2) {
	int counter = 0;
	struct Str {
		int* counter;
		~Str() {
			if (counter)
				(*counter)++;
		}
	};

	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(Str, arr); 
		int n = 4;
		for (int i = 0; i < n; ++i) {
			arr.Push(core::move(Str()));
			arr[i].counter = &counter;
		}
		arr.Clear();

		TEST_EQ(counter, n, "Clear should properly destruct elements");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayFindTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);

		for (int i = 0; i < 12; ++i) {
			arr.Push(i * 2);
		}

		int r = (int) arr.Find(4);

		TEST_LE(r, core::Array<int>::INVALID_INDEX, "Find should not return invalid index");
		TEST_L(r, (int) arr.Count(), "Find index in size");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayFindTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);

		for (int i = 0; i < 5; ++i) {
			arr.Push(i * 9);
		}

		int r = arr.Find(9999);

		TEST_EQ(r, core::Array<int>::INVALID_INDEX, "Find should return invalid index");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayReserveTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		unsigned int n = 43;
		arr.Reserve(n);

		TEST_LE(n, arr.Capacity(), "Reserve should allocate enough capacity");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayReserveTest2) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		for (int i = 0; i < 10; ++i) {
			arr.Push(i);
		}

		int cap = arr.Capacity();
		arr.Reserve(cap - 2);

		TEST_EQ(arr.Capacity(), cap, "Reserve shouldnt change capacity if its lower than current");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayBeginEndTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		for (int i = 0; i < 10; ++i) {
			arr.Push(i);
		}

		long long int dis = arr.end() - arr.begin();

		TEST_EQ(arr.Count(), dis, "Begin end distance should be same as size");
	}
	AllocatorDeinit(allocator);
}

TEST(ArrayCBeginCEndTest1) {
	DEFINE_ALLOCATOR();
	{
		DEFINE_ARRAY(int, arr);
		for (int i = 0; i < 10; ++i) {
			arr.Push(i);
		}

		long long int dis = arr.cend() - arr.cbegin();

		TEST_EQ(arr.Count(), dis, "Cbegin cend distance should be same as size");
	}
	AllocatorDeinit(allocator);
}

#undef DEFINE_ALLOCATOR
#undef DEFINE_ARRAY
