#pragma once

#include "../common/types.h"

namespace core {

	struct IAllocator;

	/**
	* Array<Type> : unordered array implementation
	* Array is not initialized in constructor, first Push will reserve UNINITIALIZED_PUSH_INIT_CAPACITY
	*/

	template<typename Type>
	class Array {
	public:
		static const u32 INVALID_INDEX = U32MAX;

	private:
		static const u32 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

	public:
		Array();
		Array(const Array<Type>& oth);
		Array(Array<Type>&& oth);
		~Array();

		void Init(IAllocator* allocator);

		// Argument cannot be the same as *this
		Array& operator=(const Array& rhs);
		Array& operator=(Array&& rhs);

		Type& operator[](u32 index);
		const Type& operator[](u32 index) const;

		bool IsEmpty() const;

		u32 Count() const;
		u32 Capacity() const;

		Type* begin();
		Type* end();

		const Type* cbegin() const;
		const Type* cend() const;

		Type& Top();

		void Pop();

		void Reserve(u32 capacity);

		void Push(const Type& value);
		void Push(Type&& value);

		void PushValues(const Type* values, u32 count);

		void Compact();

		// Swap remove, order of elements is likely to change
		void Remove(u32 index);

		void Clear();

		// Linear search with operator==, if value is not present, returns Array<Type>::INVALID_INDEX
		u32 Find(const Type& value);

	private:
		void ReservePush();

		void Reallocate(u32 capacity);

		static void Destroy(IAllocator* allocator, Type* data, u32 count);

	private:
		u32 _count;
		u32 _capacity;
		Type* _data;
		IAllocator* _allocator;
	};
}
