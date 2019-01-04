#include "world_module.h"

#include <core/common/debug.h>

#include "../engine.h"
#include "../frame/frame.h"

#include "../memory/permanent_allocator.h"

#include "engine/memory/memory_module.h"

#include "manager/scene.h"
#include "manager/CTransform2DComponentManager.h"
#include "manager/render_object_component_manager.h"

namespace eng {
	WorldModule::WorldModule() :
		_state(ModuleState::CREATED) {
	}

	WorldModule::~WorldModule() {
		_renderObjectComponents->~RenderObjectComponentManager();
		_transform2DComponents->~CTransform2DComponentManager();
		_scene->~Scene();
	}

	ModuleState WorldModule::State() {
		return _state;
	}

	void WorldModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_scene                  = PermanentNew(permanentAllocator, Scene)();
		_transform2DComponents  = PermanentNew(permanentAllocator, CTransform2DComponentManager)();
		_renderObjectComponents = PermanentNew(permanentAllocator, RenderObjectComponentManager)();
	}

	void WorldModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();

		_scene->Init(all);
		_transform2DComponents->Init(all);
		_renderObjectComponents->Init(all);

		_renderableEntitiesEvent.count = 0;
		_renderableEntitiesEvent.renderComponents = nullptr;
	}

	void WorldModule::Update(const Frame* frame) {

	}

	void WorldModule::OnEventsByType(const Frame* frame, const void* events, WorldEventType eventsType) {
		ASSERT(false);
		//switch (eventsType) {
		//default:
		//	ASSERT(false);
		//	break;
		//}
	}

	void WorldModule::QueryEventsByType(WorldEventType type, void* outEvents) {
		switch (type) {
		case WorldEventType::RENDER:
		{
			WorldRenderEvents* evt = (WorldRenderEvents*) outEvents;
			evt->renderComponents = _renderObjectComponents->RenderComponents(&evt->count);
			break;
		}
		default:
			ASSERT(false);
			break;
		}
	}

	void WorldModule::ClearPendingEvents() {
		// RENDERABLE_ENTITIES nothing to clear
	}

	Scene* WorldModule::GetScene() {
		return _scene;
	}

	CTransform2DComponentManager* WorldModule::GetTransform2DComponentManager() {
		return _transform2DComponents;
	}

	RenderObjectComponentManager* WorldModule::GetRenderObjectComponentManager() {
		return _renderObjectComponents;
	}
}