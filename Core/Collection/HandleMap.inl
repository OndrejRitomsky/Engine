#pragma once

#include "Core/Collection/HandleMap.h"

#include "Core/Common/Types.h"
#include "Core/Algorithm/Move.h"
#include "Core/Common/Handle.h"

#include "Core/Allocator/IAllocator.h"

#include "Core/Collection/Base/HandleMapBase.h"

namespace core {

	//---------------------------------------------------------------------------
	template<typename Type>
	HandleMap<Type>::HandleMap() :
		_values(nullptr),
		_allocator(nullptr),
		_base{0} {
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	HandleMap<Type>::~HandleMap() {
		for (u32 i = 0; i < _base.count; ++i)
			_values[i].~Type();

		if (_allocator)
			_allocator->Deallocate(_values);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline void HandleMap<Type>::Init(IAllocator* allocator, u16 handleType) {
		_allocator = allocator;
		_base.handleType = handleType;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type& HandleMap<Type>::Get(const Handle& handle) {
		u32 index = handle_map_base::Get(&_base, handle);
		return _values[index];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type& HandleMap<Type>::Get(const Handle& handle) const {
		u32 index = handle_map_base::Get(&_base, handle);
		return _values[index];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type* HandleMap<Type>::begin() {
		return _values;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type* HandleMap<Type>::end() {
		return &_values[_base.count];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type* HandleMap<Type>::begin() const {
		return _values;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type* HandleMap<Type>::end() const {
		return &_values[_base.count];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline bool HandleMap<Type>::IsValid(const Handle& handle) const {
		return handle_map_base::IsHandleValid(&_base, handle);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline void HandleMap<Type>::Reserve(u32 capacity) {
		if (capacity <= _base.capacity)
			return;

		HandleMapBase oldBase = _base;
		Type* oldValues = _values;

		u32 needed = capacity * sizeof(Type) + handle_map_base::SizeRequiredForCapacity(capacity);
		_values = static_cast<Type*>(_allocator->Allocate(needed, alignof(Type)));
		handle_map_base::Init(&_base, &_values[capacity], capacity);
		if (oldValues)
				handle_map_base::Copy(&_base, &oldBase);

		_base.handleType = oldBase.handleType; // this is valid even if this is first allocation
		_base.capacity = capacity;

		for (u32 i = 0; i < oldBase.count; ++i) {
			_values[i] = core::move(oldValues[i]);
			oldValues[i].~Type();
		}

		if (_allocator)
			_allocator->Deallocate(oldValues);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline u32 HandleMap<Type>::Count() const {
		return _base.count;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline u32 HandleMap<Type>::Capacity() const {
		return _base.capacity;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline typename Handle HandleMap<Type>::Add(const Type& value) {
		if (_base.count == _base.capacity)
			Reserve(_base.capacity ? _base.capacity * 2 : PUSH_INIT_CAPACITY);

		u32 index;
		Handle handle = handle_map_base::Add(&_base, index);
		_values[index] = value;
		return handle;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	void HandleMap<Type>::Remove(const Handle& handle) {
		u32 index = handle_map_base::Remove(&_base, handle);
		_values[index].~Type();
		if (index < _base.count) { // Isnt last, have to swap with the last
			_values[index] = move(_values[_base.count]);
			_values[_base.count].~Type();
		}
	}
}