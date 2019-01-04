#pragma once

#include <core/common/types.h>

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

	class RenderResourceManager;
	class RenderPipeline;

	class RenderModule {
	public:
		RenderModule();
		~RenderModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine, void* deviceContext);

		void Update(const Frame* frame);
		void OnEventsByType(const Frame* frame, const void* events, RenderEventType eventsType);
		void QueryEventsByType(RenderEventType type, void* outEvents);
		void ClearPendingEvents();

		render::IRenderDevice* RenderDevice();

	private:
		ModuleState _state;

		u8 _rendererType;

		render::IRenderDevice* _renderDevice;

		RenderPipeline* _renderPipeline;
		RenderResourceManager* _resourceManager;
	};

}