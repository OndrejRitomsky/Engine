#include "Buffer.h"

#include "Core/Common/Assert.h"
#include "Core/Algorithm/Memory.h"
#include "Core/Common/Types.h"

#include "Core/Allocator/IAllocator.h"

namespace core {
	//---------------------------------------------------------------------------
	Buffer::Buffer(IAllocator* allocator) :
		_allocator(allocator),
		_data(nullptr),
		_size(0),
		_capacity(0) {
	}
	
	//---------------------------------------------------------------------------
	Buffer::~Buffer() {
		_allocator->Deallocate(_data);
	}

	//---------------------------------------------------------------------------
	void Buffer::Init(u32 length) {
		ASSERT(length > 0);
		ASSERT(_data == nullptr);
		ASSERT(_capacity == 0);

		_data = (char*) _allocator->Allocate(length * sizeof(char), alignof(char));
		_capacity = length;
	}

	//---------------------------------------------------------------------------
	void Buffer::Append(char* data, u32 length) {
		ASSERT(_size + length <= _capacity);
		Memcpy(_data, data, length);
		_size += length;
	}

	//---------------------------------------------------------------------------
	void Buffer::Read(char* data, u32 offset, u32 length) {
		ASSERT(_size + offset + length <= _capacity);
		Memcpy(data, _data + offset, length);
	}

	//---------------------------------------------------------------------------
	char* Buffer::Data() {
		return _data;
	}

	//---------------------------------------------------------------------------
	u32 Buffer::Size() {
		return _size;
	}

	//---------------------------------------------------------------------------
	u32 Buffer::Capacity() {
		return _capacity;
	}
}
