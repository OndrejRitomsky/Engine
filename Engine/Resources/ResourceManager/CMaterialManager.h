#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/LookupArray.h>

#include "Engine/Resources/Resource/Resource.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

namespace eng {
	struct ResourceRegisterEvents;
	struct ResourceDependencyEvent;
	struct DependencyParams;
	
	class CMaterialTemplate;
	
	struct Material;
	struct MaterialDescription;
	

	class CMaterialManager {
	public:
		CMaterialManager();
		~CMaterialManager();

		void Init(core::IAllocator* allocator);

		IResourceConstructor* ResourceConstructor();
		IResourceConstructor* ResourceTemplateConstructor();

		const Material& GetMaterial(const Resource& resource) const;
		const CMaterialTemplate& GetMaterialTemplate(const Resource& resource) const;
		
		void RemoveMaterial(const Resource& resource);
		void RemoveMaterialTemplate(const Resource& resource);		

		// data will be copied
		void OnMaterialRegister(const ResourceRegisterEvents* evt);
		void OnMaterialTemplateRegister(const ResourceRegisterEvents* evt);

	private:
		// FACTORY 
		u32 DependenciesCount(const MaterialDescription* description);
		void FillDependencies(const MaterialDescription* description, ResourceDependencyEvent* inOutEvents);
		void Create(const MaterialDescription* description, const DependencyParams* dependencyParams, Resource& outHandle);

		// @TODO
		u32 TemplateDependenciesCount(const CMaterialTemplate* description);
		void TemplateFillDependencies(const CMaterialTemplate* description, ResourceDependencyEvent* inOutEvents);
		void TemplateCreate(const CMaterialTemplate* description, const DependencyParams* dependencyParams, Resource& outHandle);
		// FACTORY 

	private:
		IResourceConstructor _resourceConstructor;
		IResourceConstructor _resourceTemplateConstructor;

		core::IAllocator* _allocator;

		core::LookupArray<Material> _materials;
		core::LookupArray<CMaterialTemplate> _materialTemplates;
	};


}