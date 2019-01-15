#include "resource_module.h"

#include <core/common/pointer.h>
#include <core/common/utility.h>

#include <core/allocator/allocate.h>

#include "../engine.h"

#include "../memory/permanent_allocator.h"

#include "resource_internal_event.h"
#include "resources.h"

#include "resource_manager/render_object_manager.h"
#include "resource_manager/material_manager.h"
#include "resource_manager/texture_manager.h"
#include "resource_manager/shader_stage_manager.h"
#include "resource_manager/shader_program_manager.h"
#include "resource_manager/mesh_manager.h"
#include "resource_manager/iresource_type_factory.h"

#include "manager/resources_manager.h"
#include "manager/resources_factory.h"

#include "../memory/memory_module.h"

#include "../common/log.h"

#include "../frame/frame.h"

// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS



namespace eng {


	ResourceModule::ResourceModule() {
	}

	ResourceModule::~ResourceModule() {
		_renderObjectManager->~RenderObjectManager();
		_meshManager->~MeshManager();
		_materialManager->~MaterialManager();
		_textureManager->~TextureManager();
		_shaderProgramManager->~ShaderProgramManager();
		_shaderStageManager->~ShaderStageManager();

		_resourcesFactory->~ResourcesFactory();
		_resourcesManager->~ResourcesManager();
	}

	ModuleState ResourceModule::State() {
		return _state;
	}

	void ResourceModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_resourcesManager     = PermanentNew(permanentAllocator, ResourcesManager)();
		_resourcesFactory     = PermanentNew(permanentAllocator, ResourcesFactory)();

		_shaderStageManager   = PermanentNew(permanentAllocator, ShaderStageManager)();
		_shaderProgramManager = PermanentNew(permanentAllocator, ShaderProgramManager)();
		_textureManager       = PermanentNew(permanentAllocator, TextureManager)();
		_materialManager      = PermanentNew(permanentAllocator, MaterialManager)();
		_meshManager          = PermanentNew(permanentAllocator, MeshManager)();
		_renderObjectManager  = PermanentNew(permanentAllocator, RenderObjectManager)();
	}


	void ResourceModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::ITagAllocator* tagAllocator = engine->memoryModule->TagAllocator();
		// NOt main allocator
		core::IAllocator* allocator = engine->memoryModule->Allocator();

		_resourcesManager->Init(allocator, tagAllocator);
		_resourcesFactory->Init(allocator, tagAllocator);

		_shaderStageManager->Init(allocator);
		_shaderProgramManager->Init(allocator);
		_textureManager->Init(allocator);
		_materialManager->Init(allocator);
		_meshManager->Init(allocator);
		_renderObjectManager->Init(allocator);

		_resourcesFactory->RegisterTypeConstructor((u64)ResourceType::SHADER_PROGRAM, _shaderProgramManager->Factory());
		_resourcesFactory->RegisterTypeConstructor((u64)ResourceType::SHADER_STAGE, _shaderStageManager->Factory());
		_resourcesFactory->RegisterTypeConstructor((u64)ResourceType::MATERIAL, _materialManager->Factory());
		_resourcesFactory->RegisterTypeConstructor((u64)ResourceType::MATERIAL_TEMPLATE, _materialManager->TemplateFactory());
		_resourcesFactory->RegisterTypeConstructor((u64)ResourceType::MESH, _meshManager->Factory());
		_resourcesFactory->RegisterTypeConstructor((u64)ResourceType::RENDER_OBJECT, _renderObjectManager->Factory());
		_resourcesFactory->RegisterTypeConstructor((u64)ResourceType::TEXTURE, _textureManager->Factory());
	}

	void ResourceModule::Update(const Frame* frame) {
		ASSERT(_state == ModuleState::OK);
		ASSERT(frame);

		_resourcesFactory->Update(frame);

		{
			ResourceReadyEvents readyEvents;
			_resourcesFactory->GetReadyEvents(&readyEvents);

			if (readyEvents.count) {
				_resourcesManager->OnResourceReady(&readyEvents);
				_resourcesFactory->OnDependencyReady(&readyEvents);
			}
		}

		_resourcesManager->Update(frame);

		{
			ResourceDependencyLoadEvents loadEvents;
			_resourcesFactory->GetLoadEvents(&loadEvents);
			if (loadEvents.count) {
				_resourcesManager->RequestLoad(loadEvents.hashes, loadEvents.count, loadEvents.typesID);
			}
		}
	}

	void ResourceModule::QueryEventsByType(ResourceEventType type, void* outEvents) {
		switch (type) {
		case ResourceEventType::FS_LOAD:
			_resourcesManager->GetLoadEvents(static_cast<ResourceLoadEvents*>(outEvents));
			break;
		case ResourceEventType::LOADED:
			_resourcesManager->GetLoadedEvents(static_cast<ResourceLoadedEvents*>(outEvents));
			break;
		default:
			ASSERT(false);
			break;
		}
	}

	void ResourceModule::OnEventsByType(const Frame* frame, const void* events, ResourceEventType eventsType) {
		switch (eventsType) {
		case ResourceEventType::LOAD_ONE_TYPE:
		{
			const ResourceLoadOneTypeEvents* evt = (const ResourceLoadOneTypeEvents*)(events);
			Log("Load request frame: %llu\n", frame->frameIndex);
			_resourcesManager->RequestLoad(evt->hashes, evt->count, evt->typeID);
			break;
		}
		case ResourceEventType::DATA_LOADED:
		{
			const ResourceDataLoadedEvents* dle = (const ResourceDataLoadedEvents*)(events);
			_resourcesFactory->OnNewResource(dle);
			break;
		}
		}
	}

	void ResourceModule::ClearPendingEvents() {
		_resourcesManager->ClearPendingEvents();
		_resourcesFactory->ClearPendingEvents();
	}

	ResourcesManager* ResourceModule::GetResourcesManager() {
		return _resourcesManager;
	}

	const RenderObjectManager* ResourceModule::GetRenderObjectManager() {
		return _renderObjectManager;
	}

	const MeshManager* ResourceModule::GetMeshManager() {
		return _meshManager;
	}

	const TextureManager* ResourceModule::GetTextureManager() {
		return _textureManager;
	}

	const MaterialManager* ResourceModule::GetMaterialManager() {
		return _materialManager;
	}

	const ShaderStageManager* ResourceModule::GetShaderStageManager() {
		return _shaderStageManager;
	}

	const ShaderProgramManager* ResourceModule::GetShaderProgramManager() {
		return _shaderProgramManager;
	}
}