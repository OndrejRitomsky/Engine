#include "RenderContextStream.h"

#include <Core/Common/Utility.h>
#include <Core/Common/Assert.h>
#include <Core/Allocator/IAllocator.h>
#include <Core/Allocator/IAllocator.inl>
#include <Core/TagAllocator/SingleTagProxyAllocator.h>

namespace render {

	//---------------------------------------------------------------------------
	RenderContextStream::RenderContextStream() :
		_allocator(nullptr),
		_buffer(nullptr),
		_bufferCapacity(0),
		_bufferSize(0) {
	}

	//---------------------------------------------------------------------------
	void RenderContextStream::Init(core::SingleTagProxyAllocator* allocator) {
		_allocator = allocator;
	}

	//---------------------------------------------------------------------------
	char* RenderContextStream::ReserveSize(u64 size) {
		if (!Reserve(size))
			return nullptr;

		ASSERT(size <= _bufferCapacity - _bufferSize);

		char* res = _buffer + _bufferSize;
		_bufferSize += size;
		return res;
	}

	//---------------------------------------------------------------------------
	bool RenderContextStream::Reserve(u64 size) {
		if (size > _bufferCapacity - _bufferSize) {
			// @TODO ?? This will throw away the rest of the current buffer, might need less wasteful solution
			_buffer = (char*) _allocator->Allocate(core::Max(size, _bufferCapacity), sizeof(u64), &_bufferCapacity);
			if (!_buffer) {
				_bufferCapacity = 0;
				return false;
			}

			_bufferSize = 0;
		}

		return true;
	}
}