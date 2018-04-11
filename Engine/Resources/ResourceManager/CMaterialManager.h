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
	

	class CMaterialManager : public IResourceConstructor {
	public:
		CMaterialManager();
		~CMaterialManager();

		void Init(core::IAllocator* allocator);

		const Material& GetMaterial(const Resource& resource) const;

		void RemoveMaterial(const Resource& resource);

		// data will be copied
		void OnMaterialRegister(const ResourceRegisterEvents* evt);

	private:
		u32 DependenciesCount(const void* description);
		void FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents);
		void Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle);

	private:
		core::IAllocator* _allocator;

		core::LookupArray<Material> _materials;
	};


}