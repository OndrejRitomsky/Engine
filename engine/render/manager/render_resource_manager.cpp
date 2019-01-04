#include "render_resource_manager.h"

#include <core/common/debug.h>
#include <core/collection/hashmap.inl>
#include <core/collection/array.inl>

#include "../render_event.h"
#include "../render_internal_event.h"

#include "../../world/world_module.h"
#include "../../world/manager/render_object_component_manager.h"

#include "../../engine.h"

#include "../../memory/memory_module.h"

namespace eng {


	RenderResourceManager::RenderResourceManager() {

	}

	RenderResourceManager::~RenderResourceManager() {

	}

	void RenderResourceManager::Init(Engine* engine) {
		_resourceModule = engine->resourceModule;
		_worldModule = engine->worldModule;

		core::IAllocator* allocator = engine->memoryModule->Allocator();

		_renderComponents.Init(allocator);
		_renderables.Init(allocator);
		_renderObjects.Init(allocator);
	}

	void RenderResourceManager::OnRenderable(const RenderRenderEvents* events) {
		ASSERT(_renderComponents.Count() == 0); // -> is rendererd
		_renderComponents.PushValues(events->renderComponents, events->count);
	}


	void RenderResourceManager::Update(const Frame* frame) {
		//CTransform2DComponentManager* GetTransform2DComponentManager();
		RenderObjectComponentManager* renderComponents = _worldModule->GetRenderObjectComponentManager();

		for (const auto& component : _renderComponents) {
			h64 hash = renderComponents->ResourceHash(component);
			Resource* res = _renderObjects.Find(hash);

			if (!res) {
				//@TODO this is not know
			}
			else {
				//@TODO we know this
				RenderEntity re;
				re.renderObject = *res;
				renderComponents->GetEntity(component, &re.entity);
				_renderables.Push(re);
			}
		}

		_renderComponents.Clear();
	}

	void RenderResourceManager::GetRenderableEntitiesEvents(RenderEntityEvents* outEvents) {
		outEvents->count = _renderables.Count();
		outEvents->entities = _renderables.cbegin();
	}

	void RenderResourceManager::GetLoadRenderObjectEvents(RenderLoadRenderObjectEvents* outEvents) {
		outEvents->count = _renderables.Count();
		//outEvents->hashes = 
	}

	void RenderResourceManager::ClearPendingEvents() {

	}
}