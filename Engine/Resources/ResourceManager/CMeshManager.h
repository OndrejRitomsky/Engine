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

	class CMeshManager : public IResourceConstructor {
	public:
		CMeshManager();
		~CMeshManager();

		CMeshManager(const CMeshManager& oth) = delete;
		CMeshManager& operator=(const CMeshManager& rhs) = delete;

		void Init(core::IAllocator* allocator);
		
		const Mesh& GetMesh(const Resource& resource) const;

		void RemoveMesh(const Resource& resource);		

		// data will be copied
		void OnMeshRegister(const ResourceRegisterEvent* data, u32 eventsCount);

	private:
		u32 DependenciesCount(const void* description);
		void FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents);
		void Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle);

	private:
		core::IAllocator* _allocator;

		core::LookupArray<Mesh> _meshes;
	};


}