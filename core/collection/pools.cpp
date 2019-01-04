#include "pools.h"

#include "../common/debug.h"
#include "../allocator/allocate.h"

namespace core {

	bool BufferPool::Init(IAllocator* allocator, u32 bufferSize, u32 initialCapacity) {
		if (bufferSize < sizeof(void*)) {
			ASSERT(false);
			return false;
		}

		_used = 0;
		_bufferSize = bufferSize;

		char* first = (char*) Allocate(allocator, bufferSize, alignof(char));
		_firstFree = first;
		_lastFree = first;

		for (u32 i = 1; i < initialCapacity; ++i) {
			char* next = (char*) Allocate(allocator, bufferSize, alignof(char));
			*(void**) _lastFree = next;
			_lastFree = next;
		}

		*(void**) _lastFree = nullptr;

		_allocator = allocator;
		return true;
	}

	void BufferPool::Deinit() {
		//Assert(pool._used == 0);
		void* buffer = _firstFree;
		while (buffer) {
			void* next = *(void**) buffer;
			Deallocate(_allocator, buffer);
			buffer = next;
		}
	}

	char* BufferPool::New() {
		void* result = _firstFree;

		if (!result) {
			result = Allocate(_allocator, _bufferSize, alignof(char));
		}
		else {
			_firstFree = *(void**) _firstFree;
		}

		++_used;

		return (char*) result;
	}

	void BufferPool::Release(char* element) {
		if (!_firstFree) {
			_firstFree = element;
			_lastFree = element;
		}
		else {
			*(void**) _lastFree = element;
			_lastFree = element;
		}

		*(void**) _lastFree = nullptr;

		--_used;
	}
}