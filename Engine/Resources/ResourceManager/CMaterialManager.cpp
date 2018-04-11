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

		_allocator = allocator;

		_DependenciesCount = static_cast<IResourceConstructor::DependenciesCountFunction>(&CMaterialManager::DependenciesCount);
		_FillDependencies = static_cast<IResourceConstructor::FillDependenciesFunction>(&CMaterialManager::FillDependencies);
		_Create = static_cast<IResourceConstructor::CreateFunction>(&CMaterialManager::Create);
	}

	//---------------------------------------------------------------------------
	const Material& CMaterialManager::GetMaterial(const Resource& resource) const {
		return _materials.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::RemoveMaterial(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		_allocator->Deallocate(_materials.Get(handle).textures);
		_materials.Remove(handle);
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
	u32 CMaterialManager::DependenciesCount(const void* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CMaterialManager::FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents) {

	}

	//---------------------------------------------------------------------------
	void CMaterialManager::Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		ASSERT(dependencyParams->dependenciesCount == 2);


		//outHandle = HandleToResource(_shaderPrograms.Add(program));
	}
}

