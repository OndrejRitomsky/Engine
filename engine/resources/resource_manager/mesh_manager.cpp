#include "mesh_manager.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/lookuparray.inl>
#include <core/collection/hashmap.inl>
#include <core/algorithm/hash.h>
#include <core/algorithm/cstring.h>
#include <core/toolbox/json.h>
#include <core/allocator/allocate.h>

#include "../resource_event.h"
#include "../resource/mesh.h"
#include "../resource/resource_util.h"

#include <stddef.h>

namespace eng {

	static_assert(MESH_INDEX_BYTE_SIZE == sizeof(u16), "Mesh index size");


	static void MeshInit(Mesh* mesh, core::IAllocator* allocator, u32 verticesCount, u32 indicesCount) {
		u32 size = verticesCount * sizeof(f32) + indicesCount * sizeof(u16) + alignof(u16);

		f32* vertexData = (f32*) core::Allocate(allocator, size, alignof(f32));
		void* indexData = (void*) core::PointerAlign(vertexData + size, MESH_INDEX_BYTE_SIZE);

		mesh->vertexBuffer.bufferSize = verticesCount;
		mesh->vertexBuffer.buffer = vertexData;

		mesh->indexBuffer.bufferSize = indicesCount;
		mesh->indexBuffer.buffer = indexData;
	}

	static void MeshDeinit(Mesh* mesh, core::IAllocator* allocator) {
		core::Deallocate(allocator, mesh->vertexBuffer.buffer);
	}

	ResourceTypeFactoryParseFunction(MeshManager::ParseMesh) {
		char* text = (char*) resourceDescription;



		JsonValue value;
		if (!JsonParse(text, nullptr, &value)) {
			ASSERT(false);
		}

		if (!JsonIsObject(&value)) {
			ASSERT(false);
		}

		{
			auto kv = JsonObjectFind(&value, "path");
			if (!JsonIsString(&kv->value)) {
				ASSERT(false);
			}

			const char* str = JsonGetString(&kv->value);
			h64 path = core::Hash(str, core::Strlen(str));
			ASSERT(path == outDependencies->dependant);
		}

		JsonValue vertexArray, indexArray;

		{
			auto kv = JsonObjectFind(&value, "vertex_buffer");
			if (!JsonIsArray(&kv->value)) {
				ASSERT(false);
			}
			vertexArray = kv->value;
		}
		{
			auto kv = JsonObjectFind(&value, "index_buffer");
			if (!JsonIsArray(&kv->value)) {
				ASSERT(false);
			}
			indexArray = kv->value;
		}

		MeshManager* manager = (MeshManager*) factory;

		Mesh mesh;
		u32 vCount = JsonArrayCount(&vertexArray);
		u32 iCount = JsonArrayCount(&indexArray);
		MeshInit(&mesh, manager->_allocator, vCount, iCount);

		const JsonValue* vertex = JsonArrayCBegin(&vertexArray);
		const JsonValue* vertexEnd = vertex + vCount;

		for (u32 i = 0; vertex < vertexEnd; vertex++, i++) {
			if (!JsonIsDouble(vertex)) {
				ASSERT(false);
			}
			mesh.vertexBuffer.buffer[i] = (f32) JsonGetDouble(vertex);
		}

		const JsonValue* index = JsonArrayCBegin(&indexArray);
		const JsonValue* indexEnd = index + iCount;

		i16* indexBuffer = (i16*) mesh.indexBuffer.buffer;
		for (u32 i = 0; index < indexEnd; index++, i++) {
			if (!JsonIsInt(index)) {
				ASSERT(false);
			}
			indexBuffer[i] = (i32) JsonGetInt(index);
		}


		JsonDeinit(&value);

		manager->_pendingMeshes.Add(outDependencies->dependant, mesh);

		outDependencies->dependenciesCount = 0;
	}

	ResourceTypeFactoryCreateFunction(MeshManager::CreateMesh) {
		MeshManager* manager = (MeshManager*) factory;
		ASSERT(dependencies->dependenciesCount == 0);

		Mesh* mesh = manager->_pendingMeshes.Find(dependencies->dependant);
		ASSERT(mesh);

		Mesh newMesh = *mesh;
		manager->_pendingMeshes.SwapRemove(dependencies->dependant);

		return manager->AddMesh(newMesh);
	}


	MeshManager::MeshManager() :
		_allocator(nullptr) {
		static_assert(offsetof(MeshManager, _factoryInterface) == 0, "Factory interface has to be first");
	}

	MeshManager::~MeshManager() {
	}


	void MeshManager::Init(core::IAllocator* allocator) {
		_meshes.Init(allocator, 128);
		_pendingMeshes.Init(allocator);
		_allocator = allocator;
		_factoryInterface.Parse = ParseMesh;
		_factoryInterface.Create = CreateMesh;
	}

	IResourceTypeFactory* MeshManager::Factory() {
		return &_factoryInterface;
	}

	void MeshManager::RemoveMesh(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		auto& mesh = _meshes.Get(handle);

		MeshDeinit(&mesh, _allocator);

		_meshes.Remove(handle);
	}

	const Mesh& MeshManager::GetMesh(const Resource& resource) const {
		return _meshes.Get(ResourceToHandle(resource));
	}

	Resource MeshManager::AddMesh(const Mesh& program) {
		return HandleToResource(_meshes.Add(program));
	}

}

