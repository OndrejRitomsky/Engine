#pragma once

#include <core/collection/lookuparray.h>
#include <core/collection/hashmap.h>

#include "../resource/resource.h"

#include "iresource_type_factory.h"

namespace core {
	struct IAllocator;
}

namespace eng {
	struct Mesh;

	class MeshManager {
	public:
		MeshManager();
		~MeshManager();

		MeshManager(const MeshManager& oth) = delete;
		MeshManager& operator=(const MeshManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceTypeFactory* Factory();

		const Mesh& GetMesh(const Resource& resource) const;
		void RemoveMesh(const Resource& resource);
		Resource AddMesh(const Mesh& program);

		// data will be copied
		//void OnMeshRegister(const ResourceRegisterEvent* data, u32 eventsCount);

	private:
		static ResourceTypeFactoryParseFunction(ParseMesh);
		static ResourceTypeFactoryCreateFunction(CreateMesh);

	private:
		IResourceTypeFactory _factoryInterface;
		core::HashMap<Mesh> _pendingMeshes;
		core::IAllocator* _allocator;
		core::LookupArray<Mesh> _meshes;
	};


}