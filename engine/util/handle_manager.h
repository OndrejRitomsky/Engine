#pragma once

#include <core/common/types.h>

namespace core {
	struct IAllocator;
}

namespace eng {

	// TODO inplace linked list .. or replace in entity manage

	class HandleManager {
	public:
		static const u32 INVALID_HANDLE = U32MAX;

	public:
		HandleManager();
		~HandleManager();

		void Init(core::IAllocator* allocator);

		u32 NewHandle();
		void ReturnHandle(u32 handle);

	private:
		u32 _freeListStart;
		u32 _freeListEnd;
		u32 _freeListCount;

		u32 _size;
		u32 _capacity;

		u32* _handles;

		core::IAllocator* _allocator;
	};


}