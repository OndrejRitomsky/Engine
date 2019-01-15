#include "ringbuffer.h"

#include "../common/types.h"



template<typename Type>
RingBuffer<Type>::RingBuffer() :
	_first(0),
	_size(0) {
}


template<typename Type>
RingBuffer<Type>::~RingBuffer() {
	for (u32 i = 0; i < _size; ++i) {
		u32 k = (_first + i) % CAPACITY;
		Type* data = ((Type*) _buffer) + k;
		data->~Type();
	}
}


template<typename Type>
inline bool RingBuffer<Type>::IsFull() {
	return _size == CAPACITY;
}


template<typename Type>
inline u32 RingBuffer<Type>::NextIndex() {
	return ((u32) _first + (u32) _size) % (u32) CAPACITY;
}


template<typename Type>
inline bool RingBuffer<Type>::TryAdd(const Type& value) {
	if (IsFull()) 
		return false;

	Type* data = ((Type*) _buffer) + NextIndex();
	++_size;

	*data = value;

	return true;
}


template<typename Type>
inline bool RingBuffer<Type>::TryGet(Type& outValue) {
	if (_size == 0)
		return false;

	Type* data = ((Type*) _buffer) + _first;
	outValue = *data;
	data->~Type();

	_first = (_first + 1) % CAPACITY;
	--_size;	

	return true;
}


template<typename Type>
inline bool RingBuffer<Type>::IsEmpty() {
	return _size == 0;
}
