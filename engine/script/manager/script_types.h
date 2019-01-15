#pragma once

#include <core/common/types.h>


namespace eng {
	struct ScriptInfo;
	struct IScript;

	void ScriptTypeRegister(const ScriptInfo* info, const IScript* interf);

	bool ScriptTypeFind(const char* name, ScriptInfo* outInfo, IScript* outInterf);
}