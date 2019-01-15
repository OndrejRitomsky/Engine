#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>

#include "world_event.h" 
#include "../module.h"

namespace render {
	class IRenderDevice;
}

namespace eng {
	struct Engine;
	struct Entity;
	struct Frame;
	struct PermanentAllocator;


	//class CEntityManager;
	class Transform2DComponentManager;
	class RenderObjectComponentManager;
	class Scene;


	class ResourcesManager;

	class WorldModule {
	public:
		WorldModule();
		~WorldModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		void Update(const Frame* frame);
		void OnEventsByType(const Frame* frame, const void* events, WorldEventType eventsType);
		void QueryEventsByType(WorldEventType type, void* outEvents);
		void ClearPendingEvents();

		Scene* GetScene();
		Transform2DComponentManager* GetTransform2DComponentManager();

	private:
		Scene* _scene = nullptr;
		Transform2DComponentManager* _transform2DComponents = nullptr;

		ModuleState _state = ModuleState::CREATED;
	};

}