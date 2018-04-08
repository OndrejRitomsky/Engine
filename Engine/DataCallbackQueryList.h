#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/Array.h>
#include <Core/Collection/Function.h>

#include "Engine/FileSystem/Manager/CStringHashBank.h"
//#include "Engine/Renderer/RenderTypes.h"


namespace core {
	class IAllocator;
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
		CStringHashBank _callbackNames;
		h64Map<CallbackData> _queryList;
	};*/




}