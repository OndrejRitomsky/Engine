#include "data_callback_querylist.h"

//#include <core/allocator/iallocator.h>
#include <core/algorithm/hash.h>
#include <core/collection/hashmap.inl>


namespace eng {

	/*//---------------------------------------------------------------------------
	DataCallbackQueryList::DataCallbackQueryList(core::IAllocator* allocator) {

		_callbackNames.Init(allocator, allocator);
		_queryList.Init(allocator);
	}

	//---------------------------------------------------------------------------
	bool DataCallbackQueryList::AddQueryCallback(const char* name, u32 nameSize, UniformType type, const CallbackFuncion& cb) {
		h64 hash = core::Hash(name, nameSize);
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