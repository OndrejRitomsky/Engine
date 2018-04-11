#include "CMaterialTemplateManager.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/HashMap.inl>
#include <Core/Collection/LookupArray.inl>

#include "Engine/Resources/ResourceEvent.h"

#include "Engine/Resources/Resource/Material.h"
#include "Engine/Resources/Resource/ResourceUtil.h"
#include "Engine/Resources/Resource/Template/CMaterialTemplate.h"

#include "Engine/Resources/ResourceEvent.h"


namespace eng {
	//---------------------------------------------------------------------------
	CMaterialTemplateManager::CMaterialTemplateManager() :
		_allocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CMaterialTemplateManager::~CMaterialTemplateManager() {
	}

	//---------------------------------------------------------------------------
	void CMaterialTemplateManager::Init(core::IAllocator* allocator) {
		_materialTemplates.Init(allocator, 128);

		_allocator = allocator;

		_DependenciesCount = static_cast<IResourceConstructor::DependenciesCountFunction>(&CMaterialTemplateManager::DependenciesCount);
		_FillDependencies = static_cast<IResourceConstructor::FillDependenciesFunction>(&CMaterialTemplateManager::FillDependencies);
		_Create = static_cast<IResourceConstructor::CreateFunction>(&CMaterialTemplateManager::Create);
	}

	//---------------------------------------------------------------------------
	const CMaterialTemplate& CMaterialTemplateManager::GetMaterialTemplate(const Resource& resource) const {
		return _materialTemplates.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	void CMaterialTemplateManager::RemoveMaterialTemplate(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		_materialTemplates.Remove(handle);
	}

	//---------------------------------------------------------------------------
	void CMaterialTemplateManager::OnMaterialTemplateRegister(const ResourceRegisterEvents* evt) {
		/*for (u32 i = 0; i < eventsCount; ++i) {
		  CMaterialTemplate materialTemplate;
			Memcpy(&materialTemplate, data[i].resource, sizeof(CMaterialTemplate));

			u32 index = _materialTemplates.Add(materialTemplate);
		}*/
	}


	//---------------------------------------------------------------------------
	u32 CMaterialTemplateManager::DependenciesCount(const void* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CMaterialTemplateManager::FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents) {

	}

	//---------------------------------------------------------------------------
	void CMaterialTemplateManager::Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		ASSERT(dependencyParams->dependenciesCount == 2);


		//outHandle = HandleToResource(_shaderPrograms.Add(program));
	}


}

