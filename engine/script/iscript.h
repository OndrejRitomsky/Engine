#pragma once

#include <core/common/types.h>

#include "../world/entity.h"

namespace eng {
	struct IScript;
	struct Engine;

	typedef void(*IScriptInitFn)(void* handle, Engine* engine, Entity entity);
	typedef void(*IScriptUpdateFn)(void* handle);
	typedef void(*IScriptDeinitFn)(void* handle);

	struct IScript {
		IScriptInitFn   Init;
		IScriptUpdateFn Update;
		IScriptDeinitFn Deinit;
	};

	struct ScriptInfo {
		const char* name;
		size_t size;
		size_t alignment;
	};
}

