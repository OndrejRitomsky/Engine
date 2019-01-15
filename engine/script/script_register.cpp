#include "script_register.h"

#include "manager/script_types.h"

namespace eng {

	void RegisterScriptType(const ScriptInfo* info, const IScript* interf) {
		ScriptTypeRegister(info, interf);
	}
}
