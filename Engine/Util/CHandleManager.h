#pragma once

#include <Core/Common/Types.h>
#include <Core/Common/TypeLimits.h>

namespace core {
	class IAllocator;
}

namespace eng {

	// TODO inplace linked list .. or replace in entity manage

	class CHandleManager {
	public:
		static const u32 INVALID_HANDLE = U32MAX;

	public:
		CHandleManager();
		~CHandleManager();

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