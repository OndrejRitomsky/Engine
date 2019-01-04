#pragma once

#include <core/common/types.h>

#include "world_event.h" 
#include "../module.h"

namespace render {
	class IRenderDevice;
}

namespace eng {
	struct Engine;
	struct Frame;
	struct PermanentAllocator;

	//class CEntityManager;
	class CTransform2DComponentManager;
	class RenderObjectComponentManager;
	class Scene;

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
		CTransform2DComponentManager* GetTransform2DComponentManager();
		RenderObjectComponentManager* GetRenderObjectComponentManager();

	private:
		Scene* _scene;
		CTransform2DComponentManager* _transform2DComponents;
		RenderObjectComponentManager* _renderObjectComponents;

		ModuleState _state;
		WorldRenderEvents _renderableEntitiesEvent;
	};

}