#include "script_types.h"

#include <core/common/debug.h>
#include <core/algorithm/cstring.h>

#include "../iscript.h"


namespace eng {
	// Its registered before main, so allocator is not ready
	static const u32 SCRIPTS_CAPCITY = 128;

	struct ScriptType {
		IScript script;
		ScriptInfo info;
	};

	ScriptType* ScriptType_Storage() {
		static ScriptType scriptTypes[SCRIPTS_CAPCITY];
		return scriptTypes;
	}

	u32* ScriptType_Count() {
		static u32 count = 0;
		return &count;
	}

	void ScriptTypeRegister(const ScriptInfo* info, const IScript* interf) {
		u32* pCount = ScriptType_Count();
		u32 count = *pCount;
		if (count < SCRIPTS_CAPCITY) {
			ScriptType* scripts = ScriptType_Storage();
			scripts[count].script = *interf;
			scripts[count].info = *info;
			*pCount = count + 1;
		}
		else {
			ASSERT(false);
		}
	}


	bool ScriptTypeFind(const char* name, ScriptInfo* outInfo, IScript* outInterf) {
		u32 count = *ScriptType_Count();
		const ScriptType* data = ScriptType_Storage();
		const ScriptType* dataEnd = data + count;
		for (; data < dataEnd; data++) {
			if (core::StrEqual(name, data->info.name)) {
				*outInfo = data->info;
				*outInterf = data->script;
				return true;
			}
		}

		return false;
	}

}