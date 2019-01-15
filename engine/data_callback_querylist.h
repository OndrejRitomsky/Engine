#pragma once

#include <core/common/types.h>
#include <core/collection/array.h>
#include <core/collection/function.h>

#include "engine/file_system/manager/string_hashbank.h"
//#include "engine/renderer/rendertypes.h"


namespace core {
	struct IAllocator;
}

namespace eng {

	struct Engine;

	/*class DataCallbackQueryList {
	public:
		typedef core::Function<UniformType(EntityID entityID, Engine* engine, void* buffer, u32 bufferSize, u32& outSize)> CallbackFuncion;

	public:
		struct CallbackData {
			h64    nameHash;
			u32             index;
			UniformType     type;
			CallbackFuncion callback;
		};

	public:
		DataCallbackQueryList(core::IAllocator* allocator);

		bool AddQueryCallback(const char* name, u32 nameSize, UniformType type, const CallbackFuncion& cb);

		const CallbackData& GetQueryCallback(u64 hash);

	private:
		StringHashBank _callbackNames;
		h64Map<CallbackData> _queryList;
	};*/




}