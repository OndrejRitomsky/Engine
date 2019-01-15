#pragma once

#include "iscript.h"

#include <core/common/placement.h>

#include "../world/entity.h"


namespace eng {
	void RegisterScriptType(const ScriptInfo* info, const IScript* interf);

	struct Engine;
}

// In class declaration public scope
#define DEFINE_SCRIPT(Type) \
	static void _Script_Init(void *handle, Engine* engine, Entity entity)   { Placement(handle) Type(engine, entity); } \
	static void _Script_Update(void *handle) { ((Type*) handle)->Update(); } \
	static void _Script_Deinit(void *handle) { ((Type*) handle)->~Type(); } 

// In .cpp 
#define REGISTER_SCRIPT(Type) \
	static char _Type_registrator = []() {\
			::eng::ScriptInfo info; \
			info.name = #Type; \
			info.size = sizeof(Type); \
			info.alignment = alignof(Type); \
			::eng::IScript iscript; \
			iscript.Init   = Type::_Script_Init; \
			iscript.Update = Type::_Script_Update; \
			iscript.Deinit = Type::_Script_Deinit; \
			::eng::RegisterScriptType(&info, &iscript); \
			return 0; \
	}();

