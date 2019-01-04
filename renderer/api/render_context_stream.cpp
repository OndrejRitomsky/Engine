#include "render_context_stream.h"

#include <core/common/utility.h>
#include <core/common/debug.h>
#include <core/allocator/allocate.h>
#include <core/allocator/allocators.h>

namespace render {

	RenderContextStream::RenderContextStream() :
		_allocator(nullptr),
		_buffer(nullptr),
		_bufferCapacity(0),
		_bufferSize(0) {
	}

	void RenderContextStream::Init(core::SingleTagProxyAllocator* allocator) {
		_allocator = allocator;
	}

	char* RenderContextStream::ReserveSize(u64 size) {
		if (!Reserve(size))
			return nullptr;

		ASSERT(size <= _bufferCapacity - _bufferSize);

		char* res = _buffer + _bufferSize;
		_bufferSize += size;
		return res;
	}

	bool RenderContextStream::Reserve(u64 size) {
		if (size > _bufferCapacity - _bufferSize) {
			// @TODO ?? This will throw away the rest of the current buffer, might need less wasteful solution
			_buffer = (char*) Allocate(&_allocator->allocator, core::Max(size, _bufferCapacity), sizeof(u64), &_bufferCapacity);
			if (!_buffer) {
				_bufferCapacity = 0;
				return false;
			}

			_bufferSize = 0;
		}

		return true;
	}
}