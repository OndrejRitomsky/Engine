#include "CMeshManager.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/LookupArray.inl>
#include <Core/Allocator/IAllocator.h>

#include "Engine/Resources/ResourceEvent.h"

#include "Engine/Resources/Resource/Mesh.h"
#include "Engine/Resources/Resource/ResourceUtil.h"

namespace eng {

	//---------------------------------------------------------------------------
	CMeshManager::CMeshManager() :
		_allocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CMeshManager::~CMeshManager() {
	}

	//---------------------------------------------------------------------------
	void CMeshManager::Init(core::IAllocator* allocator) {
		_meshes.Init(allocator, 128);
		_allocator = allocator;
		_resourceConstructor._DependenciesCount = (IResourceConstructor::DependenciesCountFunction) (&CMeshManager::DependenciesCount);
		_resourceConstructor._FillDependencies = (IResourceConstructor::FillDependenciesFunction) (&CMeshManager::FillDependencies);
		_resourceConstructor._Create = (IResourceConstructor::CreateFunction) (&CMeshManager::Create);
	}

	//---------------------------------------------------------------------------
	IResourceConstructor* CMeshManager::ResourceConstructor() {
		return &_resourceConstructor;
	}

	//---------------------------------------------------------------------------
	void CMeshManager::RemoveMesh(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		auto& mesh = _meshes.Get(handle);

		_allocator->Deallocate(mesh.vertexBuffer.buffer);
		_allocator->Deallocate(mesh.indexBuffer.buffer);

		_meshes.Remove(handle);
	}

	//---------------------------------------------------------------------------
	const Mesh& CMeshManager::GetMesh(const Resource& resource) const {
		return _meshes.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	void CMeshManager::OnMeshRegister(const ResourceRegisterEvent* data, u32 eventsCount) {
		for (u32 i = 0; i < eventsCount; ++i) {
			ASSERT(false);
		/*	const Mesh* mesh = static_cast<const Mesh*>(data[i].resource);
			Mesh newMesh;
			Memcpy(&newMesh, mesh, sizeof(Mesh));

			newMesh.vertexBuffer = _allocator->Allocate(newMesh.vertexBufferSize, alignof(u64));
			newMesh.indexBuffer = _allocator->Allocate(newMesh.indexBufferSize, alignof(u32));

			Memcpy(newMesh.vertexBuffer, data[i].resource, newMesh.vertexBufferSize);
			Memcpy(newMesh.indexBuffer, data[i].resource, newMesh.indexBufferSize);

			_meshes.Add(data[i].nameHash, newMesh);*/
		}
	}

	//---------------------------------------------------------------------------
	u32 CMeshManager::DependenciesCount(const MeshDescription* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CMeshManager::FillDependencies(const MeshDescription* description, ResourceDependencyEvent* inOutEvents) {

	}

	//---------------------------------------------------------------------------
	void CMeshManager::Create(const MeshDescription* description, const DependencyParams* dependencyParams, Resource& outHandle) {



	}
}

