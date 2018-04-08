#include "CMaterialManager.h"

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
	CMaterialManager::CMaterialManager() :
		_allocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CMaterialManager::~CMaterialManager() {
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::Init(core::IAllocator* allocator) {
		_materials.Init(allocator, 128);
		_materialTemplates.Init(allocator, 128);

		_allocator = allocator;

		_resourceConstructor._DependenciesCount = (IResourceConstructor::DependenciesCountFunction) (&CMaterialManager::DependenciesCount);
		_resourceConstructor._FillDependencies = (IResourceConstructor::FillDependenciesFunction) (&CMaterialManager::FillDependencies);
		_resourceConstructor._Create = (IResourceConstructor::CreateFunction) (&CMaterialManager::Create);

		_resourceTemplateConstructor._DependenciesCount = (IResourceConstructor::DependenciesCountFunction) (&CMaterialManager::TemplateDependenciesCount);
		_resourceTemplateConstructor._FillDependencies = (IResourceConstructor::FillDependenciesFunction) (&CMaterialManager::TemplateFillDependencies);
		_resourceTemplateConstructor._Create = (IResourceConstructor::CreateFunction) (&CMaterialManager::TemplateCreate);
	}


	//---------------------------------------------------------------------------
	IResourceConstructor* CMaterialManager::ResourceConstructor() {
		return &_resourceConstructor;
	}


	//---------------------------------------------------------------------------
	IResourceConstructor* CMaterialManager::ResourceTemplateConstructor() {
		return &_resourceTemplateConstructor;
	}


	//---------------------------------------------------------------------------
	const Material& CMaterialManager::GetMaterial(const Resource& resource) const {
		return _materials.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	const CMaterialTemplate& CMaterialManager::GetMaterialTemplate(const Resource& resource) const {
		return _materialTemplates.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::RemoveMaterial(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		_allocator->Deallocate(_materials.Get(handle).textures);
		_materials.Remove(handle);
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::RemoveMaterialTemplate(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		_materialTemplates.Remove(handle);
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::OnMaterialRegister(const ResourceRegisterEvents* evt) {
		/*for (u32 i = 0; i < eventsCount; ++i) {
			// TODO material description

			const Material* material = static_cast<const Material*>(data[i].resource);
			
			Material newMaterial;
			Memcpy(&newMaterial, material, sizeof(Material));

			newMaterial.textures = static_cast<core::Handle*>(_allocator->Allocate(newMaterial.texturesCount * sizeof(core::Handle), alignof(core::Handle)));
			Memcpy(newMaterial.textures, material->textures, newMaterial.texturesCount * sizeof(core::Handle));

			_materials.Add(data[i].nameHash, newMaterial);
			ASSERT(false);
		}*/
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::OnMaterialTemplateRegister(const ResourceRegisterEvents* evt) {
		/*for (u32 i = 0; i < eventsCount; ++i) {
		  CMaterialTemplate materialTemplate;
			Memcpy(&materialTemplate, data[i].resource, sizeof(CMaterialTemplate));

			u32 index = _materialTemplates.Add(materialTemplate);
		}*/
	}


	//---------------------------------------------------------------------------
	u32 CMaterialManager::DependenciesCount(const MaterialDescription* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::FillDependencies(const MaterialDescription* description, ResourceDependencyEvent* inOutEvents) {

	}

	//---------------------------------------------------------------------------
	void CMaterialManager::Create(const MaterialDescription* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		ASSERT(dependencyParams->dependenciesCount == 2);


		//outHandle = HandleToResource(_shaderPrograms.Add(program));
	}


	//---------------------------------------------------------------------------
	u32 CMaterialManager::TemplateDependenciesCount(const CMaterialTemplate* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::TemplateFillDependencies(const CMaterialTemplate* description, ResourceDependencyEvent* inOutEvents) {

	}

	//---------------------------------------------------------------------------
	void CMaterialManager::TemplateCreate(const CMaterialTemplate* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		//ASSERT(dependencyParams->dependenciesCount == 2);

		//outHandle = HandleToResource(_shaderPrograms.Add(program));
	}

}

