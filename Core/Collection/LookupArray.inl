#pragma once

#include "LookupArray.h"

#include "Core/CoreAssert.h"
#include "Core/Allocator/IAllocator.h"

namespace core {
	//---------------------------------------------------------------------------
	template<typename Type>
	LookupArray<Type>::LookupArray() :
		_allocator(nullptr) {
		_base = {0};
		lookup_array_base::Clear(&_base);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	LookupArray<Type>::~LookupArray() {
		if (_base.data)
			_allocator->Deallocate(_base.data);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline void LookupArray<Type>::Init(IAllocator* allocator, u32 capacity) {
		ASSERT(!_allocator);
		_allocator = allocator;
		_base.elementSize = sizeof(Type);		
		_base.data = ReallocateMove<Type>(_allocator, nullptr, 0, capacity);
		_base.capacity = capacity;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline void LookupArray<Type>::Remove(u32 index) {
		static_cast<Type*>(lookup_array_base::Get(&_base, index))->~Type();
		lookup_array_base::Remove(&_base, index);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type& LookupArray<Type>::Get(u32 index) const {
		return *static_cast<const Type*>(lookup_array_base::Get(&_base, index));
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type& LookupArray<Type>::Get(u32 index) {
		return *static_cast<Type*>(lookup_array_base::Get(&_base, index));
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline u32 LookupArray<Type>::Add(const Type& value) {
		ReserveCapacity();
		u32 index = lookup_array_base::Add(&_base);
		new (lookup_array_base::Get(&_base, index)) Type(value);
		return index;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline u32 LookupArray<Type>::Add(Type&& value) {
		ReserveCapacity();
		u32 index = lookup_array_base::Add(&_base);
		new (lookup_array_base::Get(&_base, index)) Type(move(value));
		return index;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	void LookupArray<Type>::ReserveCapacity() {
		if (lookup_array_base::IsFull(&_base)) {
			_base.capacity = _base.capacity > 0 ? _base.capacity * 2 : UNINITIALIZED_PUSH_INIT_CAPACITY;
			_base.data = ReallocateMove<Type>(_allocator, static_cast<Type*>(_base.data), _base.size, _base.capacity);
		}
	}
}