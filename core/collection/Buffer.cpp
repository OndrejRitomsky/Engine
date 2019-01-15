#include "Buffer.h"

#include "../common/debug.h"
#include "../common/types.h"
#include "../common/utility.h"

#include "../algorithm/cstring.h"
#include "../allocator/allocate.h"

namespace core {
	Buffer::Buffer() :
		_allocator(nullptr),
		_data(nullptr),
		_size(0),
		_capacity(0) {
	}

	Buffer::~Buffer() {
		if (_data)
			Deallocate(_allocator, _data);
	}

	void Buffer::Init(IAllocator* allocator) {
		_allocator = allocator;
	}

	void Buffer::Reserve(u64 size) {
		if (size < _capacity)
			return;

		u64 newCapacity = Max(2 * _capacity, size);
		char* newData = (char*) Allocate(_allocator, newCapacity * sizeof(char), 128);
		
		if (_data) {
			Memcpy(newData, _data, _capacity);
			Deallocate(_allocator, _data);
		}
		_data = newData;
		_capacity = newCapacity;
	}

	void Buffer::Append(char* data, u64 size) {
		Reserve(_size + size);
		Memcpy(_data + _size, data, size);
		_size += size;
	}

	void Buffer::AppendUnchecked(char* data, u64 size) {
		ASSERT(_size + size <= _capacity);
		Memcpy(_data + _size, data, size);
		_size += size;
	}

	void Buffer::Clear() {
		_size = 0;
	}

	char* Buffer::Data() {
		return _data;
	}

	u64 Buffer::Size() {
		return _size;
	}
}
