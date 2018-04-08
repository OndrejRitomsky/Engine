#include "DataCallbackQueryList.h"

#include <Core/Allocator/IAllocator.h>
#include <Core/Algorithm/Hash.h>
#include <Core/Collection/HashMap.inl>


namespace eng {

	/*//---------------------------------------------------------------------------
	DataCallbackQueryList::DataCallbackQueryList(core::IAllocator* allocator) {

		_callbackNames.Init(allocator, allocator);
		_queryList.Init(allocator);
	}

	//---------------------------------------------------------------------------
	bool DataCallbackQueryList::AddQueryCallback(const char* name, u32 nameSize, UniformType type, const CallbackFuncion& cb) {
		h64 hash = core::ToHash(name, nameSize);
		_callbackNames.Add((ResourceHash) hash, name);

		CallbackData data;
		data.type = type;
		data.nameHash = hash;
		data.index = _queryList.Count();
		data.callback = cb;

		_queryList.Add(hash, core::move(data));

		return true;
	}

	//---------------------------------------------------------------------------
	const DataCallbackQueryList::CallbackData& DataCallbackQueryList::GetQueryCallback(u64 hash) {
		auto* fn = _queryList.Find(hash);
		ASSERT(fn);
		return *fn;
	}*/
}