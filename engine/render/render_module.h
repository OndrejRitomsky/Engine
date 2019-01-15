#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>

#include "../module.h"
#include "render_event.h"

namespace render {
	class IRenderDevice;
}
namespace core {
	struct ITagAllocator;
}

namespace eng {
	struct Frame;
	struct Engine;
	struct PermanentAllocator;

	class RenderPipeline;
	class RenderObjectComponentManager;
	class ResourcesManager;

	class RenderModule {
	public:
		RenderModule();
		~RenderModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine, void* deviceContext);

		void SetViewPort(i32 width, i32 height); // @TODO remove

		void Update(const Frame* frame);

		void OnEventsByType(const Frame* frame, const void* events, RenderEventType eventsType);
		void QueryEventsByType(RenderEventType type, void* outEvents);
		void ClearPendingEvents();

		// @TODO not needed?
		const render::IRenderDevice* RenderDevice() const;

		RenderObjectComponentManager* GetRenderObjectComponentManager();

	private:
		ModuleState _state = ModuleState::CREATED;
		u8 _rendererType = 0;


		// Should be in scene I guess // this doesnt have priority first the resources pipeline have to be proper
		ResourcesManager* _resourcesManager = nullptr;
		core::HashMap<h64> _pendingResources; // Entity to resource hash
		//

		render::IRenderDevice* _renderDevice = nullptr;
		RenderObjectComponentManager* _renderObjectComponents = nullptr;
		RenderPipeline* _renderPipeline = nullptr;
	};

}