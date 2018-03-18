#pragma once

#include "HashMap.h"
#include "Core/Utility.h"
#include "Core/Allocator/IAllocator.h"
#include "Core/Algorithm/Hash.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

namespace core {
	template<typename Value>
	class HashMap<Value>::ConstKeyValueIterator {
	public:
		ConstKeyValueIterator(const HashMap& _map, u32 index);
		ConstKeyValueIterator& operator++();
		ConstKeyValueIterator& operator--();
		bool operator==(const ConstKeyValueIterator& rhs) const;
		bool operator!=(const ConstKeyValueIterator& rhs) const;
		ConstKeyValueIterator operator+(u32 rhs) const;
		ConstKeyValueIterator operator-(u32 rhs) const;
		const ConstKeyValueIterator& operator*();
	public:
		Hash key;
		Value& value;
	private:
		const HashMap& _map;
		u32 _index;
	};

	//---------------------------------------------------------------------------
	template<typename Value>
	HashMap<Value>::HashMap() {
		memset(this, 0, sizeof(HashMap<Value>));
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	HashMap<Value>::HashMap(const HashMap& oth) :
		_allocator(oth._allocator) {

		Allocate(oth._capacity);
		for (u32 i = 0; i < oth._base.count; ++i)
			Add(oth._base.keys[i], oth._base.values[i]);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	HashMap<Value>::HashMap(HashMap&& oth) :
		_allocator(move(oth._allocator)) {

		hash_map_base::MoveyArrays(&_base, oth._base);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	HashMap<Value>::~HashMap() {
		if (_allocator)
			_allocator->Deallocate(_base._data);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	HashMap<Value>& HashMap<Value>::operator=(const HashMap& rhs) {
		for (u32 i = 0; i < _base.count; ++i)
			(static_cast<Value*>(_base.values) + i)->~Value();

		void* oldData = _base->_data;
		IAllocator* oldAllocator = _allocator;
		memset(this, 0, sizeof(HashMap<Value>));

		_allocator = rhs._allocator;
		_base = Allocate(rhs.capacity);
		hash_map_base::CopyArrays(&_base, rhs._base);

		oldAllocator->Deallocate(oldData);
		return *this;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	HashMap<Value>& HashMap<Value>::operator=(HashMap&& rhs) {
		for (u32 i = 0; i < _base.count; ++i)
			(static_cast<Value*>(_base.values) + i)->~Value();

		void* oldData = _base._data;
		IAllocator* oldAllocator = _allocator;
		memset(this, 0, sizeof(HashMap<Value>));

		_allocator = rhs._allocator;
		_base = Allocate(rhs._base.capacity);
		hash_map_base::MoveyArrays(&_base, rhs._base); // Not sure if sending rvalue is valid

		oldAllocator->Deallocate(oldData);
		return *this;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	void HashMap<Value>::Init(IAllocator* allocator) {
		_allocator = allocator;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline const Value* HashMap<Value>::Find(Hash hash) const {
		return static_cast<const Value*>(hash_map_base::Find(&_base, hash, sizeof(Value)));
	}

  //---------------------------------------------------------------------------
  template<typename Value>
  inline Value* HashMap<Value>::Find(Hash hash) {
    return static_cast<Value*>(hash_map_base::Find(&_base, hash, sizeof(Value)));
  }

	//---------------------------------------------------------------------------
	template<typename Value>
	inline Value* HashMap<Value>::begin() {
		return static_cast<Value*>(_base.values);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline Value* HashMap<Value>::end() {
		return static_cast<Value*>(_base.values) + _base.count;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline const Value* HashMap<Value>::cbegin() const {
		return static_cast<Value*>(_base.values);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline const Value* HashMap<Value>::cend() const {
		return static_cast<Value*>(_base.values) + _base.count;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline const Hash* HashMap<Value>::cKeyBegin() const {
		return _base.keys;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline const Hash* HashMap<Value>::cKeyEnd() const {
		return _base.keys + _base.count;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline typename HashMap<Value>::ConstKeyValueIterator HashMap<Value>::cKeyValueBegin() const {
		return ConstKeyValueIterator(*this, 0);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline typename HashMap<Value>::ConstKeyValueIterator HashMap<Value>::cKeyValueEnd() const {
		return ConstKeyValueIterator(*this, _base.count);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline u32 HashMap<Value>::Count() const {
		return _base.count;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline bool HashMap<Value>::Add(Hash hash, const Value& value) {
		u32 index;
		bool ok = InternalAdd(hash, index);
		//ASSERT(ok || (!ok && value == *(static_cast<Value*>(_base.values) + index)));// it was not inside, or it was inside and the value is the same as the one added
		if (ok)
			new (static_cast<Value*>(_base.values) + index) Value(value);
		return ok;
	}

	template<typename Value>
	inline 	bool HashMap<Value>::Add(Hash hash, Value&& value) {
		u32 index;
		bool ok = InternalAdd(hash, index);
		//ASSERT(ok || (!ok && value == *(static_cast<Value*>(_base.values) + index)));// it was not inside, or it was inside and the value is the same as the one added
		if (ok)
			new (static_cast<Value*>(_base.values) + index) Value(move(value));
		return ok;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline bool HashMap<Value>::Remove(Hash hash) {
		u32 index;
		if (!hash_map_base::Remove(&_base, hash, index))
			return false;

		--_base.count;
		Value* values = static_cast<Value*>(_base.values);
		values[index].~Value();
		if (index < _base.count) { // Isnt last, have to swap with the last
			values[index] = move(values[_base.count]);
			values[_base.count].~Value();
		}
		return true;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline void HashMap<Value>::Clear() {
		u32 count = Count();
		while (count > 0) {
			Hash hash = _base.keys[count - 1];
			Remove(hash);
			count = Count();
		}
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline void HashMap<Value>::Rehash(u32 capacity) {
		for (u32 i = 0; i < _base.count; ++i)
			static_cast<Value*>(_base.values)[i].~Value();

		HashMapBase oldBase = _base;
		_base = Allocate(capacity);

		for (u32 i = 0; i < oldBase.count; ++i)
			Add(oldBase.keys[i], move(static_cast<Value*>(oldBase.values)[i]));

		_allocator->Deallocate(oldBase._data);
	}

	template<typename Value>
	inline HashMapBase HashMap<Value>::Allocate(u32 capacity) {
		u32 needed = capacity * (2 * sizeof(u32) + sizeof(Hash) + sizeof(Value));
		needed += 2 * alignof(u32) + alignof(Hash) + alignof(Value);
		return hash_map_base::Create(_allocator->Allocate(needed, 8), capacity, alignof(Value));
	}

	template<typename Value>
	inline bool HashMap<Value>::InternalAdd(Hash hash, u32& outIndex) {
		const f32 FULL_RATIO = 0.6f;
		// this will reallocate even if the element is already inside
		if (_base.count >= (u32) (_base.capacity * FULL_RATIO))
			Rehash(_base.capacity == 0 ? PUSH_INIT_CAPACITY : _base.capacity * 2);

		if (!hash_map_base::Add(&_base, hash, outIndex))
			return false;

		++_base.count;
		return true;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	HashMap<Value>::ConstKeyValueIterator::ConstKeyValueIterator(const HashMap& map, u32 index) :
		_map(map),
		key(map._base.keys[index]),
		value(static_cast<Value*>(map._base.values)[index]),
		_index(index) {
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline typename HashMap<Value>::ConstKeyValueIterator& HashMap<Value>::ConstKeyValueIterator::operator++() {
		_index++;
		key = _map._base.keys[_index];
		value = static_cast<Value*>(_map._base.values)[_index];
		return *this;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline typename HashMap<Value>::ConstKeyValueIterator& HashMap<Value>::ConstKeyValueIterator::operator--() {
		_index--;
		key = _map._base.keys[_index];
		value = static_cast<Value*>(_map._base.values)[_index];
		return *this;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline bool typename HashMap<Value>::ConstKeyValueIterator::operator==(const ConstKeyValueIterator& rhs) const {
		return _index == rhs._index;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline bool typename HashMap<Value>::ConstKeyValueIterator::operator!=(const ConstKeyValueIterator& rhs) const {
		return _index != rhs._index;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline typename const HashMap<Value>::ConstKeyValueIterator::ConstKeyValueIterator& HashMap<Value>::ConstKeyValueIterator::operator*() {
		return *this;
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline typename HashMap<Value>::ConstKeyValueIterator HashMap<Value>::ConstKeyValueIterator::operator+(u32 rhs) const {
		return ConstKeyValueIterator(_map, _index + rhs);
	}

	//---------------------------------------------------------------------------
	template<typename Value>
	inline typename HashMap<Value>::ConstKeyValueIterator HashMap<Value>::ConstKeyValueIterator::operator-(u32 rhs) const {
		return ConstKeyValueIterator(_map, _index - rhs);
	}

}