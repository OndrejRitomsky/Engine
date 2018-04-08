#pragma once

#include "Core/Common/Types.h"
#include "Core/Parallel/Locks/LockUtil.h"


// thread safe fixed queue

namespace core {
	template<typename Type, typename Lock, u16 CAPACITY>
	class ThreadSafeFixedQueue {
	public:
		ThreadSafeFixedQueue();
		ThreadSafeFixedQueue(const ThreadSafeFixedQueue& oth) = delete;
		ThreadSafeFixedQueue operator=(const ThreadSafeFixedQueue& rhs) = delete;
		~ThreadSafeFixedQueue();

		bool TryAdd(const Type& value);
		bool TryGet(Type& outValue);
		bool IsEmpty();

	private:
		bool IsFullUnsafe();

		u16 NextIndexUnsafe();

	private:
		u16 _first;
		u16 _size;

		Lock _lock;

		char _buffer[CAPACITY * sizeof(Type)];
	};
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	template<typename Type, typename Lock, u16 CAPACITY>
	ThreadSafeFixedQueue<Type, Lock, CAPACITY>::ThreadSafeFixedQueue() :
		_first(0),
		_size(0) {
	}

	//---------------------------------------------------------------------------
	template<typename Type, typename Lock, u16 CAPACITY>
	ThreadSafeFixedQueue<Type, Lock, CAPACITY>::~ThreadSafeFixedQueue() {
		for (u16 i = 0; i < _size; ++i) {
			u16 k = (_first + i) % CAPACITY;
			Type* data = ((Type*) _buffer) + k;
			data->~Type();
		}
	}


	//---------------------------------------------------------------------------
	template<typename Type, typename Lock, u16 CAPACITY>
	inline bool ThreadSafeFixedQueue<Type, Lock, CAPACITY>::IsFullUnsafe() {
		return _size == CAPACITY;
	}

	//---------------------------------------------------------------------------
	template<typename Type, typename Lock, u16 CAPACITY>
	inline u16 ThreadSafeFixedQueue<Type, Lock, CAPACITY>::NextIndexUnsafe() {
		return (u16) (((u32) _first + (u32) _size) % (u32) CAPACITY);
	}

	//---------------------------------------------------------------------------
	template<typename Type, typename Lock, u16 CAPACITY>
	inline bool ThreadSafeFixedQueue<Type, Lock, CAPACITY>::TryAdd(const Type& value) {
		LockGuard<Lock> guard(_lock);

		if (IsFullUnsafe()) 
			return false;

		Type* data = ((Type*) _buffer) + NextIndexUnsafe();
		++_size;

		*data = value; // Copy assignment must be locked too, so get isnt called on invalid memory	

		return true;
	}

	//---------------------------------------------------------------------------
	template<typename Type, typename Lock, u16 CAPACITY>
	inline bool ThreadSafeFixedQueue<Type, Lock, CAPACITY>::TryGet(Type& outValue) {
		LockGuard<Lock> guard(_lock);

		if (_size == 0)
			return false;

		Type* data = ((Type*) _buffer) + _first;
		outValue = *data;
		data->~Type();

		_first = (_first + 1) % CAPACITY;
		--_size;	

		return true;
	}

	//---------------------------------------------------------------------------
	template<typename Type, typename Lock, u16 CAPACITY>
	inline bool ThreadSafeFixedQueue<Type, Lock, CAPACITY>::IsEmpty() {
		LockGuard<Lock> guard(_lock);
		return _size == 0;
	}
}