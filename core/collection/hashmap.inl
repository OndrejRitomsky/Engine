#pragma once

#include "hashmap.h"

#include "../common/types.h"
#include "../common/debug.h"
#include "../common/placement.h"

#include "../algorithm/cstring.h"
#include "../algorithm/move.h"

#include "../allocator/allocate.h"

namespace core {
	template<typename Value>
	HashMap<Value>::HashMap() :
		_allocator(nullptr),
		_values(nullptr),
		_base{0} {
	}

	template<typename Value>
	HashMap<Value>::HashMap(const HashMap& oth) :
		_allocator(oth._allocator) {

		Allocate(oth._capacity);
		for (u32 i = 0; i < oth._base.count; ++i)
			Add(oth._base.keys[i], oth._values[i]);
	}

	template<typename Value>
	HashMap<Value>::HashMap(HashMap&& oth) :
		_allocator(move(oth._allocator)),
		_values(oth._values) {

		hash_map_base::Move(&_base, core::move(oth._base));

		oth._allocator = nullptr;
		oth._values = nullptr;
	}

	template<typename Value>
	HashMap<Value>::~HashMap() {
		if (_values)
			Deallocate(_allocator, _values);
	}

	template<typename Value>
	HashMap<Value>& HashMap<Value>::operator=(const HashMap& rhs) {
		if (rhs._values == _values)
			return *this;

		Clear();
		_allocator->Deallocate(_values);

		_allocator = rhs._allocator;
		Allocate(rhs._base.capacity);
		hash_map_base::Copy(&_base, rhs._base);

		for (u32 i = 0; i < rhs._base.count; ++i)
			_values[i] = rhs._values[i];

		return *this;
	}

	template<typename Value>
	HashMap<Value>& HashMap<Value>::operator=(HashMap&& rhs) {
		if (rhs._values == _values)
			return *this;

		Clear();
		Deallocate(_allocator, _values);

		_allocator = rhs._allocator;
		Allocate(rhs._base.capacity);
		hash_map_base::Move(&_base, core::move(rhs._base));

		for (u32 i = 0; i < rhs._base.count; ++i)
			_values[i] = core::move(rhs._values[i]);

		rhs._allocator = nullptr;
		rhs._values = nullptr;
		return *this;
	}

	template<typename Value>
	void HashMap<Value>::Init(IAllocator* allocator) {
		_allocator = allocator;
	}

	template<typename Value>
	inline const Value* HashMap<Value>::Find(h64 hash) const {
		u32 index = hash_map_base::Find(&_base, hash);
		return index == HashMapBase::INVALID_INDEX ? nullptr : &_values[index];
	}

	template<typename Value>
	inline Value* HashMap<Value>::Find(h64 hash) {
		u32 index = hash_map_base::Find(&_base, hash);
		return index == HashMapBase::INVALID_INDEX ? nullptr : &_values[index];
	}

	template<typename Value>
	inline Value* HashMap<Value>::begin() {
		return _values;
	}

	template<typename Value>
	inline Value* HashMap<Value>::end() {
		return &_values[_base.count];
	}

	template<typename Value>
	inline const Value* HashMap<Value>::begin() const {
		return _values;
	}

	template<typename Value>
	inline const Value* HashMap<Value>::end() const {
		return &_values[_base.count];
	}

	template<typename Value>
	inline const h64* HashMap<Value>::beginKey() const {
		return _base.keys;
	}

	template<typename Value>
	inline const h64* HashMap<Value>::endKey() const {
		return &_base.keys[_base.count];
	}

	template<typename Value>
	inline typename HashMap<Value>::KeyValueIterator HashMap<Value>::Iterator() {
		KeyValueIterator it;
		it.keys = _base.keys;
		it.values = _values;
		it.count = _base.count;
		return it;
	}

	template<typename Value>
	inline const typename HashMap<Value>::ConstKeyValueIterator HashMap<Value>::CIterator() const {
		ConstKeyValueIterator it;
		it.keys = _base.keys;
		it.values = _values;
		it.count = _base.count;
		return it;
	}

	template<typename Value>
	inline u32 HashMap<Value>::Count() const {
		return _base.count;
	}

	template<typename Value>
	inline bool HashMap<Value>::Add(h64 hash, const Value& value) {
		if (hash_map_base::IsFull(&_base))
			IncreaseCapacity();

		u32 index = hash_map_base::Add(&_base, hash);
		if (index == HashMapBase::INVALID_INDEX)
			return false;

		Placement(&_values[index]) Value(value);
		return true;
	}

	template<typename Value>
	inline bool HashMap<Value>::Add(h64 hash, Value&& value) {
		// this will reallocate even if the element is already inside
		// @TODO check
		if (hash_map_base::IsFull(&_base))
			IncreaseCapacity();

		u32 index = hash_map_base::Add(&_base, hash);
		if (index == HashMapBase::INVALID_INDEX)
			return false;

		Placement(&_values[index]) Value(move(value));
		return true;
	}

	template<typename Value>
	inline bool HashMap<Value>::SwapRemove(h64 hash) {
		u32 index = hash_map_base::SwapRemove(&_base, hash);
		if (index == HashMapBase::INVALID_INDEX)
			return false;

		_values[index].~Value();
		if (index < _base.count) { // Isnt last, have to swap with the last
			_values[index] = move(_values[_base.count]);
			_values[_base.count].~Value();
		}
		return true;
	}

	template<typename Value>
	inline void HashMap<Value>::Clear() {
		for (u32 i = 0; i < _base.count; ++i)
			_values[i].~Value();

		hash_map_base::Clear(&_base);
	}

	template<typename Value>
	inline void HashMap<Value>::IncreaseCapacity() {
		u32 capacity = _base.capacity ? _base.capacity * 2 : PUSH_INIT_CAPACITY;

		HashMapBase oldBase = _base;
		Value* oldValues = _values;

		Allocate(capacity);

		for (u32 i = 0; i < oldBase.count; ++i) {
			Add(oldBase.keys[i], move(oldValues[i]));
			oldValues[i].~Value();
		}

		if (_allocator)
			Deallocate(_allocator, oldValues);
	}

	template<typename Value>
	inline void HashMap<Value>::Allocate(u32 capacity) {
		u32 needed = capacity * sizeof(Value) + hash_map_base::SizeRequiredForCapacity(capacity);
		_values = static_cast<Value*>(core::Allocate(_allocator, needed, alignof(Value)));
		hash_map_base::Init(&_base, &_values[capacity], capacity);
	}
}