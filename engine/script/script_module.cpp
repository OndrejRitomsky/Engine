#include "script_module.h"

#include <core/common/debug.h>
#include <core/allocator/allocate.h>
#include <core/collection/hashmap.inl>

#include "../engine.h"
#include "../world/entity.h"
#include "../frame/frame.h"

#include "../memory/permanent_allocator.h"
#include "../memory/memory_module.h"

#include "manager/script_component_manager.h"


namespace eng {
	ScriptModule::ScriptModule() {}

	ScriptModule::~ScriptModule() {
		_scriptsComponentManager->~ScriptComponentManager();
	}

	ModuleState ScriptModule::State() {
		return _state;
	}

	void ScriptModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_scriptsComponentManager = PermanentNew(permanentAllocator, ScriptComponentManager)();
	}

	void ScriptModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();

		_scriptsComponentManager->Init(engine, all);

	}

	void ScriptModule::Update(const Frame* frame) {
		ASSERT(_state == ModuleState::OK);
		_scriptsComponentManager->Update();
	}


	ScriptComponentManager* ScriptModule::GetScriptComponentManager() {
		return _scriptsComponentManager;
	}

}