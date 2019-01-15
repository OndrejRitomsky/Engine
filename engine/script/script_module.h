#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>

//#include "world_event.h" 
#include "../module.h"

namespace render {
	class IRenderDevice;
}

namespace eng {
	struct Engine;
	struct Entity;
	struct Frame;
	struct PermanentAllocator;

	class ScriptComponentManager;

	class ScriptModule {
	public:
		ScriptModule();
		~ScriptModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		void Update(const Frame* frame);
		//void OnEventsByType(const Frame* frame, const void* events, ScriptEventType eventsType);
		//void QueryEventsByType(ScriptEventType type, void* outEvents);
		//void ClearPendingEvents();

		ScriptComponentManager* GetScriptComponentManager();

	private:
		ScriptComponentManager* _scriptsComponentManager = nullptr;
		ModuleState _state = ModuleState::CREATED;
	};

}