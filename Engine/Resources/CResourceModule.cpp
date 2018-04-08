#include "CResourceModule.h"

#include <Core/Common/Pointer.h>
#include <Core/Common/Utility.h>

#include <Core/TagAllocator/ITagAllocator.h>

#include "Engine/Engine.h"

#include "Engine/Modules/CStaticConstructor.h"

#include "Engine/Resources/ResourceInternalEvent.h"

#include "Engine/Resources/ResourceManager/CRenderObjectManager.h"
#include "Engine/Resources/ResourceManager/CMaterialManager.h"
#include "Engine/Resources/ResourceManager/CTextureManager.h"
#include "Engine/Resources/ResourceManager/CShaderStageManager.h"
#include "Engine/Resources/ResourceManager/CShaderProgramManager.h"
#include "Engine/Resources/ResourceManager/CMeshManager.h"
#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

#include "Engine/Resources/Manager/CResourcesLoader.h"
#include "Engine/Resources/Manager/CResourcesManager.h"
#include "Engine/Resources/Manager/CResourcesFactory.h"

#include "Engine/Memory/CMemoryModule.h"

#include "Engine/Frame/Frame.h"

// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS
// @TODO MEMORY MANAGMENT IN MANAGERS


namespace eng {




	//---------------------------------------------------------------------------
	CResourceModule::CResourceModule() :
		_state(ModuleState::CREATED),
		_textureManager(nullptr),
		_meshManager(nullptr),
		_shaderStageManager(nullptr),
		_shaderProgramManager(nullptr),
		_materialManager(nullptr),
		_resourcesManager(nullptr),
		_tagAllocator(nullptr),
		_resourcesLoader(nullptr),
		_resourcesFactory(nullptr)
	{

	}

	//---------------------------------------------------------------------------
	CResourceModule::~CResourceModule() {
		_renderObjectManager->~CRenderObjectManager();
		_meshManager->~CMeshManager();
		_materialManager->~CMaterialManager();
		_textureManager->~CTextureManager();
		_shaderProgramManager->~CShaderProgramManager();
		_shaderStageManager->~CShaderStageManager();

		_resourcesFactory->~CResourcesFactory();
		_resourcesManager->~CResourcesManager();
		_resourcesLoader->~CResourcesLoader();
	}

	//---------------------------------------------------------------------------
	ModuleState CResourceModule::State() {
		return _state;
	}

	//---------------------------------------------------------------------------
	void CResourceModule::ConstructSubsytems(CStaticConstructor* constructor) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_resourcesLoader = constructor->Construct<CResourcesLoader>();
		_resourcesManager = constructor->Construct<CResourcesManager>();
		_resourcesFactory = constructor->Construct<CResourcesFactory>();

		_shaderStageManager = constructor->Construct<CShaderStageManager>();
		_shaderProgramManager = constructor->Construct<CShaderProgramManager>();
		_textureManager = constructor->Construct<CTextureManager>();
		_materialManager = constructor->Construct<CMaterialManager>();
		_meshManager = constructor->Construct<CMeshManager>();
		_renderObjectManager = constructor->Construct<CRenderObjectManager>();
	}

	//---------------------------------------------------------------------------
	void CResourceModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		_tagAllocator = engine->memoryModule->TagAllocator();

		// NOt main allocator
		core::IAllocator* allocator = engine->memoryModule->Allocator();

		_resourcesLoader->Init(allocator);
		_resourcesManager->Init(allocator);
		_resourcesFactory->Init(allocator, _tagAllocator);

		_textureManager->Init(allocator);
		_shaderStageManager->Init(allocator);
		_shaderProgramManager->Init(allocator);
		_materialManager->Init(allocator);
		_meshManager->Init(allocator);
		_renderObjectManager->Init(allocator);

		_resourcesFactory->RegisterTypeConstructor(static_cast<u64>(ResourceType::MATERIAL), _materialManager->ResourceConstructor());
		_resourcesFactory->RegisterTypeConstructor(static_cast<u64>(ResourceType::MATERIAL_TEMPLATE), _materialManager->ResourceTemplateConstructor());
		_resourcesFactory->RegisterTypeConstructor(static_cast<u64>(ResourceType::MESH), _meshManager->ResourceConstructor());
		_resourcesFactory->RegisterTypeConstructor(static_cast<u64>(ResourceType::RENDER_OBJECT), _renderObjectManager->ResourceConstructor());
		_resourcesFactory->RegisterTypeConstructor(static_cast<u64>(ResourceType::SHADER_PROGRAM), _shaderProgramManager->ResourceConstructor());
		_resourcesFactory->RegisterTypeConstructor(static_cast<u64>(ResourceType::SHADER_STAGE), _shaderStageManager->ResourceConstructor());
		_resourcesFactory->RegisterTypeConstructor(static_cast<u64>(ResourceType::TEXTURE), _textureManager->ResourceConstructor());

	}

	//---------------------------------------------------------------------------
	void CResourceModule::Update(const Frame* frame) {
		ASSERT(_state == ModuleState::OK);
		ASSERT(frame);

		_resourcesLoader->Update();
		_resourcesFactory->Update(frame);

		UpdateResourcesFactoryEvents(frame);
	}

	//---------------------------------------------------------------------------
	void CResourceModule::QueryEventsByType(ResourceEventType type, void* outEvents) {
		switch (type) {
		case ResourceEventType::LOAD:
			_resourcesLoader->GetLoadEvents(static_cast<ResourceLoadEvents*>(outEvents));
			break;
		case ResourceEventType::LOADED:
			_resourcesLoader->GetLoadedEvents(static_cast<ResourceLoadedEvents*>(outEvents));
			break;
		default:
			ASSERT(false);
			break;
		}
	}

	// When there is even more events move this to some common place or file)
	//---------------------------------------------------------------------------
	void OnResourceRegister(CResourcesLoader* loader, CResourcesFactory* factory, const ResourceRegisterEvents* evt) {
		ResourceDataLoadedEvents dle = *evt;
		factory->OnNewResource(&dle);

		ResourcePreloadEvents pe;
		pe.count = evt->count;
		pe.hashes = evt->hashes;
		pe.typeIDs = evt->typeIDs;
		loader->OnResourcePreload(&pe);
	}

	//---------------------------------------------------------------------------
	void CResourceModule::OnEventsByType(const void* events, ResourceEventType eventsType) {
		switch (eventsType) {
		case ResourceEventType::REGISTER:
		{
			const ResourceRegisterEvents* pe = static_cast<const ResourceRegisterEvents*>(events);
			OnResourceRegister(_resourcesLoader, _resourcesFactory, pe);
			break;
		}
		case ResourceEventType::PRELOAD:
		{
			const ResourcePreloadEvents* pe = static_cast<const ResourcePreloadEvents*>(events);
			_resourcesLoader->RequestLoad(pe->hashes, pe->count, pe->typeIDs);
			break;
		}
		case ResourceEventType::DATA_LOADED:
		{
			const ResourceDataLoadedEvents* dle = static_cast<const ResourceDataLoadedEvents*>(events);
			_resourcesFactory->OnNewResource(dle);
			break;
		}
		}
	}

	//---------------------------------------------------------------------------
	void CResourceModule::ClearPendingEvents() {
		_resourcesLoader->ClearPendingEvents();
		_resourcesFactory->ClearPendingEvents();
	}

	//---------------------------------------------------------------------------
	void CResourceModule::UpdateResourcesFactoryEvents(const Frame* frame) {
		ResourceReadyEvents readyEvents;
		_resourcesFactory->GetReadyEvents(&readyEvents);

		if (readyEvents.count) {
			_resourcesManager->OnResourceReady(&readyEvents);
			_resourcesLoader->OnResourceReady(&readyEvents);

			// It could have been dependency, (could have been handled inside, but keep this for now)
			_resourcesFactory->OnDependencyReady(&readyEvents);
		}

		ResourceDependencyLoadEvents evt;
		_resourcesFactory->GetLoadEvents(&evt);
		if (evt.count) {
			// Discriminated size for buffer (part resources, part types);
			u64 maxSize = core::Max(sizeof(Resource), sizeof(u64));
			u64 maxAlign = core::Max(alignof(Resource), alignof(u64));

			u64 size = evt.count * (sizeof(h64) + sizeof(bool) + maxSize) + alignof(bool) + maxAlign;
			h64* hashes = static_cast<h64*>(_tagAllocator->AllocateWithTag(frame->gameTag, size, alignof(h64), nullptr));
			bool* has = static_cast<bool*>(core::mem::Align(&hashes[evt.count], alignof(bool)));
			void* extra = core::mem::Align(&has[evt.count], maxAlign);

			_resourcesManager->HasResources(evt.hashes, evt.count, has);

			u32 hasCount = 0;
			u32 loadCount = 0;
			u32 index;
			for (u32 i = 0; i < evt.count; ++i) {
				if (has[i]) {
					hashes[hasCount] = evt.hashes[i]; // Put ready hashes to the front
					++hasCount;
				}
				else {
					index = evt.count - loadCount - 1;
					hashes[index] = evt.hashes[i]; // Put load hashes to the back
					static_cast<u64*>(extra)[index] = evt.typesID[i];
					++loadCount;
				}
			}

			if (hasCount) {
				Resource* resources = static_cast<Resource*>(extra);
				_resourcesManager->AcquireResource(hashes, hasCount, resources);

				ResourceReadyEvents ready = {hasCount, hashes, resources};
				_resourcesFactory->OnDependencyReady(&ready);
			}

			if (loadCount) {
				u32 loadIndex = evt.count - loadCount;
				_resourcesLoader->RequestLoad(&hashes[loadIndex], loadCount, &static_cast<const u64*>(extra)[loadIndex]);
			}
		}
	}

	//---------------------------------------------------------------------------
	CResourcesLoader* CResourceModule::ResourcesLoader() {
		return _resourcesLoader;
	}

	//---------------------------------------------------------------------------
	CResourcesManager* CResourceModule::ResourcesManager() {
		return _resourcesManager;
	}

	//---------------------------------------------------------------------------
	const CRenderObjectManager* CResourceModule::RenderObjectManager() {
		return _renderObjectManager;
	}

	//---------------------------------------------------------------------------
	const CMeshManager* CResourceModule::MeshManager() {
		return _meshManager;
	}

	//---------------------------------------------------------------------------
	const CTextureManager* CResourceModule::TextureManager() {
		return _textureManager;
	}

	//---------------------------------------------------------------------------
	const CShaderStageManager* CResourceModule::ShaderStageManager() {
		return _shaderStageManager;
	}

	//---------------------------------------------------------------------------
	const CShaderProgramManager* CResourceModule::ShaderProgramManager() {
		return _shaderProgramManager;
	}

	//---------------------------------------------------------------------------
	const CMaterialManager* CResourceModule::MaterialManager() {
		return _materialManager;
	}

}