#include "render_context_stream.h"

#include <core/common/utility.h>
#include <core/common/pointer.h>
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

	char* RenderContextStream::ReserveSize(u64 size, u64 alignment) {
		u64 needed = size + alignment;
		if (needed > _bufferCapacity - _bufferSize) {
				// @TODO ?? This will throw away the rest of the current buffer, might need less wasteful solution
				_buffer = (char*) Allocate(&_allocator->allocator, core::Max(needed, _bufferCapacity), 128, &_bufferCapacity);
				if (!_buffer) {
					_bufferCapacity = 0;
					return nullptr;
				}

				_bufferSize = 0;
		}

		ASSERT(needed <= _bufferCapacity - _bufferSize);

		char* res = _buffer + _bufferSize;
		res = (char*) core::PointerAlign(res, alignment);
		
		ASSERT(((u64) res) % alignment == 0);
		_bufferSize += needed; // @TODO we dont have to skip all its wastefull
		return res;
	}
}