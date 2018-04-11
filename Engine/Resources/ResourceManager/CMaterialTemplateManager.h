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
	

	class CMaterialTemplateManager : public IResourceConstructor {
	public:
		CMaterialTemplateManager();
		~CMaterialTemplateManager();

		void Init(core::IAllocator* allocator);

		const CMaterialTemplate& GetMaterialTemplate(const Resource& resource) const;

		void RemoveMaterialTemplate(const Resource& resource);

		// data will be copied
		void OnMaterialTemplateRegister(const ResourceRegisterEvents* evt);

	private:
		u32 DependenciesCount(const void* description);
		void FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents);
		void Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle);

	private:
		core::IAllocator* _allocator;

		core::LookupArray<Material> _materials;
		core::LookupArray<CMaterialTemplate> _materialTemplates;
	};


}