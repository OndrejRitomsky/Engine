#pragma once

#include <core/collection/lookuparray.h>
#include <core/collection/hashmap.h>

#include "../resource.h"

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

	private:
		static ResourceTypeFactoryParseFunction(TryCreateMesh);
		static ResourceTypeFactoryCreateFunction(CreateMesh);

	private:
		IResourceTypeFactory _factoryInterface;
		core::IAllocator* _allocator;
		core::LookupArray<Mesh> _meshes;
	};


}