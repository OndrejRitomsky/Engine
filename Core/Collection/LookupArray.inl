#pragma once

#include "Core/Collection/LookupArray.h"

#include "Core/Algorithm/Move.h"
#include "Core/Common/Assert.h"
#include "Core/Common/Placement.h"

#include "Core/Allocator/IAllocator.h"

namespace core {
	template<typename Type>
	u32 LookupArray<Type>::Conversion(Handle handle) const {
		ASSERT(handle < (u64) (0xFFffFFff));
		return static_cast<u32>(handle);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	LookupArray<Type>::LookupArray() :
		_allocator(nullptr) {

		ASSERT(sizeof(Type) >= sizeof(u32));

		in_place_linked_list::Init(&_linked, nullptr);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	LookupArray<Type>::~LookupArray() {
		ASSERT(_count == 0); // cant call destructors, array doesnt know where holes are
		if (_linked.data)
			_allocator->Deallocate(_linked.data);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline void LookupArray<Type>::Init(IAllocator* allocator, u32 capacity) {
		ASSERT(!_allocator);
		_allocator = allocator;

		in_place_linked_list::Init(&_linked, _allocator->Allocate(capacity * sizeof(Type), alignof(Type)));
		_capacity = capacity;
		_count = 0;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline void LookupArray<Type>::Remove(Handle handle) {
		u32 index = Conversion(handle);
		static_cast<Type*>(_linked.data)[index].~Type();
		in_place_linked_list::Remove(&_linked, index, sizeof(Type));
		--_count;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type& LookupArray<Type>::Get(Handle handle) const {
		return static_cast<const Type*>(_linked.data)[Conversion(handle)];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type& LookupArray<Type>::Get(Handle handle) {
		return static_cast<Type*>(_linked.data)[Conversion(handle)];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Handle LookupArray<Type>::Add(const Type& value) {
		ReserveCapacity();
		u64 index = in_place_linked_list::Add(&_linked, sizeof(Type));
		Placement(&static_cast<Type*>(_linked.data)[index]) Type(value);
		++_count;
		return index;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Handle LookupArray<Type>::Add(Type&& value) {
		ReserveCapacity();
		u64 index = in_place_linked_list::Add(&_linked, sizeof(Type));
		Placement(&static_cast<Type*>(_linked.data)[index]) Type(move(value));
		++_count;
		return index;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	void LookupArray<Type>::ReserveCapacity() {
		if (_linked.countWithHoles == _capacity) {
			_capacity = _capacity > 0 ? _capacity * 2 : UNINITIALIZED_PUSH_INIT_CAPACITY;

			Type* result = static_cast<Type*>(_allocator->Allocate(_capacity * sizeof(Type), alignof(Type)));

			for (u32 i = 0; i < _count; ++i) {
				result[i] = move(static_cast<Type*>(_linked.data)[i]);
				static_cast<Type*>(_linked.data)[i].~Type();
			}

			if (_linked.data)
				_allocator->Deallocate(_linked.data);

			_linked.data = result;
		}
	}
}