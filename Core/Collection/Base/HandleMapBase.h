#pragma once

#include "../../common/types.h"
namespace core {

	static const u32 INTERNAL_INVALID_INDEX = U32MAX;

	struct HandleMapBase {
		union InternalHandle {
			struct {
				u32 index;
				u16 generation;
				u16 type : 15;
				u16 free : 1;
			} internal;

			Handle value;
		};

		enum { INVALID_INDEX = U32MAX };
		u16 handleType;

		u32 freeHandleFirst;
		u32 freeHandleLast;

		u32 count;
		u32 capacity;

		u32* handleIndices; // extra indirection so data can be contiguous
		InternalHandle* handles;
	};

	namespace handle_map_base {
		void Init(HandleMapBase* base, void* data, u32 capacity);

		Handle Add(HandleMapBase* base, u32& outIndex);

		u32 Remove(HandleMapBase* base, const Handle& handle);

		u32 Get(const HandleMapBase* base, const Handle& handle);

		bool IsHandleValid(const HandleMapBase* base, const Handle& handle);

		u32 SizeRequiredForCapacity(u32 capacity);

		void Copy(HandleMapBase* dest, HandleMapBase* source);
	}
}