#include "world_module.h"

#include <core/common/debug.h>
#include <core/allocator/allocate.h>
#include <core/collection/hashmap.inl>

#include "entity.h"
#include "../engine.h"
#include "../frame/frame.h"

#include "../memory/permanent_allocator.h"
#include "../memory/memory_module.h"

#include "manager/scene.h"
#include "manager/transform2d_component_manager.h"

#include "../resources/resource_module.h"
#include "../resources/manager/resources_manager.h"


namespace eng {
	WorldModule::WorldModule() {}

	WorldModule::~WorldModule() {
		_transform2DComponents->~Transform2DComponentManager();
		_scene->~Scene();
	}

	ModuleState WorldModule::State() {
		return _state;
	}

	void WorldModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_scene                  = PermanentNew(permanentAllocator, Scene)();
		_transform2DComponents  = PermanentNew(permanentAllocator, Transform2DComponentManager)();
	}

	void WorldModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();

		_scene->Init(all);
		_transform2DComponents->Init(all);

	}

	void WorldModule::Update(const Frame* frame) {
	

	}

	void WorldModule::OnEventsByType(const Frame* frame, const void* events, WorldEventType eventsType) {

	}

	void WorldModule::QueryEventsByType(WorldEventType type, void* outEvents) {

	}

	void WorldModule::ClearPendingEvents() {
	}

	Scene* WorldModule::GetScene() {
		return _scene;
	}

	Transform2DComponentManager* WorldModule::GetTransform2DComponentManager() {
		return _transform2DComponents;
	}
}