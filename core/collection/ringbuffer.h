#pragma once

#include "../common/types.h"


template<typename Type>
class RingBuffer {
public:
	static const u32 CAPACITY = 1024;

public:
	RingBuffer();
	RingBuffer(const RingBuffer& oth) = delete;
	RingBuffer operator=(const RingBuffer& rhs) = delete;
	~RingBuffer();

	bool TryAdd(const Type& value);
	bool TryGet(Type& outValue);
	bool IsEmpty();
	bool IsFull();

private:
	u32 NextIndex();

private:
	u32 _first;
	u32 _size;

	char _buffer[CAPACITY * sizeof(Type)];
};
