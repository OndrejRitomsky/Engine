#include "render_module.h"

#include <core/common/debug.h>

#include <core/allocator/allocate.h>
#include <core/collection/hashmap.inl>
#include <core/algorithm/hash.h>

#include <renderer/api/irender_device.h>
#include <renderer/gl/gl_render_device.h>

#include "../world/entity.h"
#include "../engine.h"
#include "../frame/frame.h"
#include "../common/log.h"
#include "../memory/permanent_allocator.h"
#include "../memory/memory_module.h"

#include "pipeline/render_pipeline.h"
#include "manager/render_object_component_manager.h"

#include "../resources/resource_module.h"
#include "../resources/manager/resources_manager.h"
#include "../resources/resource_manager/render_object_manager.h"


namespace eng {

	enum class RendererType : u8 {
		INVALID = 0,
		OPEN_GL
	};

	RenderModule::RenderModule() {
	}

	RenderModule::~RenderModule() {
		_renderObjectComponents->~RenderObjectComponentManager();
		_renderDevice->~IRenderDevice();
	}

	ModuleState RenderModule::State() {
		return _state;
	}

	void RenderModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_renderDevice = (render::IRenderDevice*)(PermanentNew(permanentAllocator, render::GLRenderDevice)());
		_rendererType = static_cast<u8>(RendererType::OPEN_GL);

		_renderPipeline = PermanentNew(permanentAllocator, RenderPipeline)();
		_renderPipeline->ConstructSubsytems(permanentAllocator);

		_renderObjectComponents = PermanentNew(permanentAllocator, RenderObjectComponentManager)();

	}

	void RenderModule::Init(Engine* engine, void* deviceContext) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();

		static_cast<render::GLRenderDevice*>(_renderDevice)->Init(all, all, core::Hash, deviceContext); // We need type identifier

		_renderPipeline->Init(engine);
		_renderObjectComponents->Init(all);
		_pendingResources.Init(all);

		_resourcesManager = engine->resourceModule->GetResourcesManager();
	}

	void RenderModule::SetViewPort(i32 width, i32 height) {
		_renderPipeline->SetViewPort(width, height);
	}

	void RenderModule::Update(const Frame* frame) {
		{ // @TODO move somewhere (maybe pipeline maybe not)
			u32 renderEntitiesCount;
			const RenderObjectComponent* renderEntities = _renderObjectComponents->RenderComponents(&renderEntitiesCount);

			if (renderEntitiesCount > 0) {
				RenderObjectComponent* readyRenderEntities = (RenderObjectComponent*) TagAllocate(frame->tagAllocator, frame->gameTag,
					sizeof(RenderObjectComponent) * renderEntitiesCount, alignof(RenderObjectComponent));
				u32 readyRenderEntitiesCount = 0;

				for (u32 i = 0; i < renderEntitiesCount; ++i) {
					auto component = renderEntities[i];
					if (_renderObjectComponents->GetReady(component)) {
						readyRenderEntities[readyRenderEntitiesCount++] = component;
					}
					else {
						h64 resourceHash = _renderObjectComponents->GetResourceHash(component);
						Entity entity = _renderObjectComponents->GetEntity(component);

						bool has;
						_resourcesManager->HasResources(&resourceHash, 1, &has);

						if (!has) {
							if (!_pendingResources.Find(entity.Hash())) {
								_resourcesManager->RequestLoad(&resourceHash, 1, (u64) ResourceType::RENDER_OBJECT);
								_pendingResources.Add(entity.Hash(), resourceHash);
							}
						}
						else {
							// @TODO this does resource counting its bad here? ... it should be somewhere said that we know or dont know it
							Resource resource;
							_resourcesManager->AcquireResource(&resourceHash, 1, &resource);
							_renderObjectComponents->SetReady(component, true);
							_renderObjectComponents->SetResource(component, resource);
							readyRenderEntities[readyRenderEntitiesCount++] = component;
						}
					}
				}

				Log("Rendering %u \n", readyRenderEntitiesCount);
				_renderPipeline->Render(_renderDevice, frame, readyRenderEntities, readyRenderEntitiesCount);
			}
		}
	}

	void RenderModule::OnEventsByType(const Frame* frame, const void* events, RenderEventType eventsType) {
		switch (eventsType) {
		case RenderEventType::RESOURCES_LOADED:
		{
			RenderResourcesLoadedEvents* evt = (RenderResourcesLoadedEvents*) events;
			// @TODO move to update
			// @TODO how to do this fast.. one resource can be waited by more entities.. and probably will (Extra data is dumb solution)

			for (u32 i = 0; i < evt->count; ++i) {
				h64 resourceHash = evt->hashes[i];

				auto it = _pendingResources.CIterator();
				for (u32 j = 0; j < it.count; ++j) {
					if (it.values[j] == resourceHash) {
						h64 entityHash = it.keys[j];
						Entity entity = Entity::FromHash(entityHash);

						RenderObjectComponent component;
						if (!_renderObjectComponents->Find(entity, &component)) {
							ASSERT(false);
						}
						Resource resource;
						_resourcesManager->AcquireResource(&resourceHash, 1, &resource);
						_renderObjectComponents->SetReady(component, true);
						_renderObjectComponents->SetResource(component, resource);
						_pendingResources.SwapRemove(entityHash);
					}
				}
			}
			break;
		}
		default:
			ASSERT(false);
			break;
		}
	}

	void RenderModule::QueryEventsByType(RenderEventType type, void* outEvents) {

	}

	void RenderModule::ClearPendingEvents() {

	}

	const render::IRenderDevice* RenderModule::RenderDevice() const {
		return _renderDevice;
	}

	RenderObjectComponentManager* RenderModule::GetRenderObjectComponentManager() {
		return _renderObjectComponents;
	}
}
