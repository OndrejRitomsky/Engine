#include "handle_manager.h"

#include <core/algorithm/cstring.h>
#include <core/common/debug.h>
#include <core/allocator/allocate.h>

namespace eng {
	static const u32 INITIAL_CAPACITY = 256u;
	static const u32 MINIMAL_FREE_LIST_COUNT = 100;

	// Index is returned and on its index is this value (or its free list if its not used index)
	static const u32 USED_HANDLE_VALUE = HandleManager::INVALID_HANDLE - 1;

	static inline u32* Reallocate(core::IAllocator* allocator, u32* data, u32 size, u32 newCapacity) {
		u32* newData = static_cast<u32*>(Allocate(allocator, newCapacity * sizeof(u32), alignof(u32)));

		if (data) {
			core::Memcpy(newData, data, size * sizeof(u32));
			Deallocate(allocator, data);
		}

		return newData;
	}

	HandleManager::HandleManager() :
		_freeListStart(INVALID_HANDLE),
		_freeListEnd(INVALID_HANDLE),
		_size(0),
		_capacity(0),
		_freeListCount(0),
		_handles(nullptr),
		_allocator(nullptr) {
	}

	HandleManager::~HandleManager() {
		Deallocate(_allocator, _handles);
		ASSERT(_size == _freeListCount);
	}

	void HandleManager::Init(core::IAllocator* allocator) {
		_allocator = allocator;

		_handles = Reallocate(_allocator, nullptr, 0, INITIAL_CAPACITY);
		_capacity = INITIAL_CAPACITY;
	}

	u32 HandleManager::NewHandle() {
		u32 result;
		if (_freeListStart == INVALID_HANDLE || _freeListCount < MINIMAL_FREE_LIST_COUNT) {

			if (_size == _capacity) {
				ASSERT(_capacity);
				_capacity *= 2;
				_handles = Reallocate(_allocator, _handles, _size, _capacity);
			}

			result = _size;
			_handles[result] = USED_HANDLE_VALUE;
			++_size;
		}
		else {
			result = _freeListStart;
			_freeListStart = _handles[result];
			_handles[result] = USED_HANDLE_VALUE;
			--_freeListCount;
		}

		return result;
	}

	void HandleManager::ReturnHandle(u32 handle) {
		ASSERT(_handles[handle] == USED_HANDLE_VALUE);

		if (_freeListStart == INVALID_HANDLE) {
			ASSERT(_freeListEnd == INVALID_HANDLE);
			_freeListStart = handle;
			_handles[_freeListStart] = INVALID_HANDLE;

			_freeListEnd = _freeListStart;
		}
		else {
			ASSERT(_freeListEnd != INVALID_HANDLE);
			ASSERT(_freeListEnd < _size);

			_handles[_freeListEnd] = handle;
			_handles[handle] = INVALID_HANDLE;
			_freeListEnd = handle;
		}
		++_freeListCount;
	}
}