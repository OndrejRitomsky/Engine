#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>
#include <core/collection/array.h>

#include "../../resources/resource/resource.h"

namespace eng {
	struct Engine;
	struct RenderEntityEvents;
	struct RenderRenderEvents;
	struct RenderLoadRenderObjectEvents;
	struct RenderObjectComponent;
	struct Frame;
	struct RenderEntity;

	class ResourceModule;
	class WorldModule;


	class RenderResourceManager {
	public:
		RenderResourceManager();
		~RenderResourceManager();

		void Init(Engine* engine);

		void OnRenderable(const RenderRenderEvents* events);
		void Update(const Frame* frame);

		void GetRenderableEntitiesEvents(RenderEntityEvents* outEvents);
		void GetLoadRenderObjectEvents(RenderLoadRenderObjectEvents* outEvents);

		void ClearPendingEvents();

	private:
		WorldModule * _worldModule;
		ResourceModule* _resourceModule;

		core::HashMap<Resource> _renderObjects; // known and loaded
		core::Array<RenderObjectComponent> _renderComponents;
		core::Array<RenderEntity> _renderables;
	};
}

