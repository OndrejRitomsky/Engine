#pragma once

#include "../common/types.h"
#include "../common/placement.h"
#include "../common/debug.h"

#include "../algorithm/cstring.h"
#include "../algorithm/move.h"
#include "../allocator/allocate.h"


namespace core {
	template<typename Type>
	Array<Type>::Array() :
		_count(0),
		_capacity(0),
		_data(0),
		_allocator(nullptr) {
	}

	template<typename Type>
	Array<Type>::~Array() {
		Destroy(_allocator, _data, _count);
	}

	template<typename Type>
	Array<Type>::Array(const Array<Type>& oth) :
		_allocator(oth._allocator),
		_capacity(0),
		_count(0),
		_data(nullptr) {
		Reserve(oth._capacity);

		_capacity = oth._capacity;
		_count = oth._count;

		for (u32 i = 0; i < _count; ++i)
			_data[i] = oth._data[i];
	}

	template<typename Type>
	Array<Type>::Array(Array<Type>&& oth) :
		_data(oth._data),
		_count(oth._count),
		_capacity(oth._capacity),
		_allocator(move(oth._allocator)) {
		oth._data = nullptr;
		oth._count = 0;
		oth._capacity = 0;
		oth._allocator = nullptr;
	}


	template<typename Type>
	void Array<Type>::Init(IAllocator* allocator) {
		_allocator = allocator;
	}

	template<typename Type>
	Array<Type>& Array<Type>::operator=(const Array<Type>& rhs) {
		Type* oldData = _data;
		u32 oldCount = _count;
		IAllocator* oldAllocator = _allocator;
		Memset(this, 0, sizeof(Array<Type>));

		_allocator = rhs._allocator;
		Reallocate(rhs._capacity);

		_count = rhs._count;
		for (u32 i = 0; i < _count; ++i)
			_data[i] = rhs._data[i];

		Destroy(oldAllocator, oldData, oldCount);
		return *this;
	}

	template<typename Type>
	Array<Type>& Array<Type>::operator=(Array<Type>&& rhs) {
		Type* oldData = _data;
		u32 oldCount = _count;
		IAllocator* oldAllocator = _allocator;
		Memset(this, 0, sizeof(Array<Type>));

		_data = rhs._data;
		_count = rhs._count;
		_capacity = rhs._capacity;
		_allocator = move(rhs._allocator);

		Destroy(oldAllocator, oldData, oldCount);

		rhs._allocator = nullptr;
		rhs._data = nullptr;
		rhs._count = 0;
		rhs._capacity = 0;

		return *this;
	}

	template<typename Type>
	inline Type& Array<Type>::operator[](u32 index) {
		ASSERT(index < _count);
		return _data[index];
	}

	template<typename Type>
	inline const Type& Array<Type>::operator[](u32 index) const {
		ASSERT(index < _count);
		return _data[index];
	}

	template<typename Type>
	inline bool Array<Type>::IsEmpty() const {
		return _count == 0;
	}

	template<typename Type>
	inline u32 Array<Type>::Count() const {
		return _count;
	}

	template<typename Type>
	inline u32 Array<Type>::Capacity() const {
		return _capacity;
	}

	template<typename Type>
	inline Type* Array<Type>::begin() {
		return _data;
	}

	template<typename Type>
	inline const Type* Array<Type>::cbegin() const {
		return _data;
	}

	template<typename Type>
	inline Type* Array<Type>::end() {
		return _data + _count;
	}

	template<typename Type>
	inline const Type* Array<Type>::cend() const {
		return _data + _count;
	}

	template<typename Type>
	inline void Array<Type>::Compact() {
		Reallocate(_count);
	}

	template<typename Type>
	inline void Array<Type>::Reserve(u32 capacity) {
		if (capacity <= _capacity)
			return;

		Reallocate(capacity);
	}

	template<typename Type>
	inline void Array<Type>::Push(const Type& value) {
		ReservePush();
		Placement(_data + _count++) Type(value);
	}

	template<typename Type>
	inline void Array<Type>::Push(Type&& value) {
		ReservePush();
		Placement(_data + _count++) Type(move(value));
	}

	template<typename Type>
	inline void Array<Type>::PushValues(const Type* values, u32 count) {
		u32 doubleCapacity = 2 * _capacity;
		u32 needed = _count + count;
		Reserve(doubleCapacity > needed ? doubleCapacity : needed);
		for (u32 i = 0; i < count; ++i)
			Placement(_data + _count + i) Type(values[i]);

		_count += count;
	}

	template<typename Type>
	inline void Array<Type>::Remove(u32 index) {
		ASSERT(index < _count);
		--_count;

		_data[index].~Type();
		if (index < _count) {
			_data[index] = move(_data[_count]);
			_data[_count].~Type();
		}
	}
	template<typename Type>
	inline Type& Array<Type>::Top() {
		ASSERT(_count > 0);
		return _data[_count - 1];
	}

	template<typename Type>
	inline void Array<Type>::Pop() {
		ASSERT(_count > 0);
		--_count;
		_data[_count].~Type();
	}

	template<typename Type>
	inline void Array<Type>::Clear() {
		for (u32 i = 0; i < _count; ++i)
			_data[i].~Type();

		_count = 0;
	}

	template<typename Type>
	inline u32 Array<Type>::Find(const Type& value) {
		for (u32 i = 0; i < _count; ++i) {
			if (_data[i] == value)
				return i;
		}
		return INVALID_INDEX;
	}

	template<typename Type>
	inline void Array<Type>::ReservePush() {
		if (_count == _capacity)
			Reallocate(_capacity == 0 ? UNINITIALIZED_PUSH_INIT_CAPACITY : _capacity * 2);
	}

	template<typename Type>
	void Array<Type>::Reallocate(u32 capacity) {
		if (capacity < _count)
			return;

		_capacity = capacity;
		Type* result = static_cast<Type*>(Allocate(_allocator, capacity * sizeof(Type), alignof(Type)));
		for (u32 i = 0; i < _count; ++i) {
			result[i] = move(_data[i]);
			_data[i].~Type();
		}

		if (_data)
			Deallocate(_allocator, _data);

		_data = result;
	}

	template<typename Type>
	void Array<Type>::Destroy(IAllocator* allocator, Type* data, u32 count) {
		if (data) {
			for (u32 i = 0; i < count; ++i)
				data[i].~Type();
			Deallocate(allocator, data);
		}
	}

}