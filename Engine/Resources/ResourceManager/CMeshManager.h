#pragma once

#include <Core/Collection/LookupArray.h>

#include "Engine/Resources/Resource/Resource.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

namespace core {
	class IAllocator;
}

namespace eng {
	struct ResourceRegisterEvent;
	struct ResourceDependencyEvent;
	struct DependencyParams;

	struct Mesh;
	struct MeshDescription;

	class CMeshManager {
	public:
		CMeshManager();
		~CMeshManager();

		CMeshManager(const CMeshManager& oth) = delete;
		CMeshManager& operator=(const CMeshManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceConstructor* ResourceConstructor();

		const Mesh& GetMesh(const Resource& resource) const;

		void RemoveMesh(const Resource& resource);		

		// data will be copied
		void OnMeshRegister(const ResourceRegisterEvent* data, u32 eventsCount);

	private:
		// FACTORY 
		u32 DependenciesCount(const MeshDescription* description);
		void FillDependencies(const MeshDescription* description, ResourceDependencyEvent* inOutEvents);
		void Create(const MeshDescription* description, const DependencyParams* dependencyParams, Resource& outHandle);
		// FACTORY 

	private:
		IResourceConstructor _resourceConstructor;
		core::IAllocator* _allocator;

		core::LookupArray<Mesh> _meshes;
	};


}