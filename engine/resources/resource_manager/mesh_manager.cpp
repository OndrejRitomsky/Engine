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
#include "../resources.h"
#include "../resource_util.h"

#include <stddef.h>

namespace eng {

	static_assert(MESH_INDEX_BYTE_SIZE == sizeof(u16), "Mesh index size");

	static f32 JsonGetFloatOrInt(const JsonValue* value) {
		if (JsonIsDouble(value)) {
			return (f32) JsonGetDouble(value);
		}
		else if (JsonIsInt(value)) {
			return (f32) JsonGetInt(value);
		}
		ASSERT(false);
		return 0.0f;
	}

	static bool JsonGetArray(JsonValue* value, const char* name, JsonValue* outArray) {
		JsonKeyValue* kv = JsonObjectFind(value, name);

		if (!kv)
			return false;

		if (!JsonIsArray(&kv->value))
			ASSERT(false);

		*outArray = kv->value;
		return true;
	}

	static void JsonArrayCopyToU16Buffer(JsonValue* array, void* buffer, u32 count) {
		if (buffer == nullptr)
			return;

		u32 arrayCount = JsonArrayCount(array);
		ASSERT(arrayCount == count);
		const JsonValue* value = JsonArrayCBegin(array);
		const JsonValue* valueEnd = value + arrayCount;

		u16* indexBuffer = (u16*) buffer;
		for (u32 i = 0; value < valueEnd; value++, i++)
			indexBuffer[i] = (u16) JsonGetInt(value);
	}


	static void JsonArrayCopyToBuffer(JsonValue* array, void* buffer, u32 count, MeshBufferType type) {
		if (buffer == nullptr)
			return;

		u32 arrayCount = JsonArrayCount(array);
		const JsonValue* value = JsonArrayCBegin(array);
		const JsonValue* valueEnd = value + arrayCount;

		if (type == MeshBufferType::V2) {
			ASSERT(arrayCount == count * 2);

			f32* vertexBuffer = (f32*) buffer;
			for (u32 i = 0; value < valueEnd; value++, i++)
				vertexBuffer[i] = JsonGetFloatOrInt(value);
		}
		else {
			ASSERT(false);
		}
	}

	static void MeshBufferTypeMemInfo(MeshBufferType type, u32* outSize, u32* outAlignment) {
		if (type == MeshBufferType::INVALID) {
			*outSize = 0;
			*outAlignment = 0;
		}
		else if (type == MeshBufferType::V2) {
			*outSize = sizeof(f32) * 2;
			*outAlignment = alignof(f32);
		}
		else {
			ASSERT(false);
		}
	}

	static char* MeshSetBuffer(char* buffer, void** bufferToSet, MeshBufferType type, u32 size, u32 alignment, u32 count) {
		if (type != MeshBufferType::INVALID) {
			char* alignedBuffer = (char*) core::PointerAlign(buffer, alignment);
			*bufferToSet = alignedBuffer;
			return alignedBuffer + count * size;
		}
		
		*bufferToSet = nullptr;
		return buffer;
	}

	static char* MeshAllocateBuffers(Mesh* mesh, core::IAllocator* allocator) {
		u32 posSize, posAlignment;
		u32 normalSize, normalAlignment;
		u32 uvsSize, uvsAlignment;
		u32 colorSize, colorAlignment;

		MeshBufferTypeMemInfo(mesh->positionsType, &posSize, &posAlignment);
		MeshBufferTypeMemInfo(mesh->normalsType, &normalSize, &normalAlignment);
		MeshBufferTypeMemInfo(mesh->uvsType, &uvsSize, &uvsAlignment);
		MeshBufferTypeMemInfo(mesh->colorType, &colorSize, &colorAlignment);

		u32 verticesSize = mesh->verticesCount * (posSize + normalSize + uvsSize + colorSize) +
			posAlignment + normalAlignment + uvsAlignment + colorAlignment;

		u32 indicesSize = mesh->indiciesCount * sizeof(u16) + alignof(u16);

		char* data = (char*) core::Allocate(allocator, verticesSize + indicesSize, 1);
		char* result = data;

		mesh->data = data;

		ASSERT(mesh->positionsType != MeshBufferType::INVALID);
		ASSERT(mesh->indiciesCount > 0);

		data = MeshSetBuffer(data, &mesh->positions, mesh->positionsType, posSize, posAlignment, mesh->verticesCount);
		data = MeshSetBuffer(data, &mesh->normals, mesh->normalsType, normalSize, normalAlignment, mesh->verticesCount);
		data = MeshSetBuffer(data, &mesh->uvs, mesh->uvsType, uvsSize, uvsAlignment, mesh->verticesCount);
		data = MeshSetBuffer(data, &mesh->colors, mesh->colorType, colorSize, colorAlignment, mesh->verticesCount);

		mesh->indices = core::PointerAlign(data, alignof(u16));
		
		return result;
	}

	static void MeshDeinit(Mesh* mesh, core::IAllocator* allocator) {
		core::Deallocate(allocator, mesh->data);
	}

	ResourceTypeFactoryParseFunction(MeshManager::TryCreateMesh) {
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
			if (!kv || !JsonIsString(&kv->value)) {
				ASSERT(false);
			}

			const char* str = JsonGetString(&kv->value);
			h64 path = core::Hash(str, core::Strlen(str));
			ASSERT(path == package->dependant.hash);
		}

		JsonValue vertexArray;
		JsonValue indexArray;
		JsonValue uvsArray;
		//JsonValue normalsArray;
		//JsonValue colorsArray;

		Mesh mesh;

		
		if (!JsonGetArray(&value, "positions", &vertexArray))
			ASSERT(false);

		if (!JsonGetArray(&value, "indices", &indexArray))
			ASSERT(false);

		mesh.positionsType = MeshBufferType::V2;
		mesh.normalsType = MeshBufferType::INVALID;
		mesh.uvsType = JsonGetArray(&value, "uvs", &uvsArray) ? MeshBufferType::V2 : MeshBufferType::INVALID;
		mesh.colorType = MeshBufferType::INVALID;


		auto kv = JsonObjectFind(&value, "vertices_count");
		if (!kv || !JsonIsInt(&kv->value))
			ASSERT(false);

		mesh.indiciesCount = JsonArrayCount(&indexArray);
		mesh.verticesCount = (u32) JsonGetInt(&kv->value);

		MeshManager* manager = (MeshManager*) factory;

		mesh.data = MeshAllocateBuffers(&mesh, manager->_allocator);

		JsonArrayCopyToBuffer(&vertexArray, mesh.positions, mesh.verticesCount, mesh.positionsType);
		JsonArrayCopyToU16Buffer(&indexArray, mesh.indices, mesh.indiciesCount);
		JsonArrayCopyToBuffer(&uvsArray, mesh.uvs, mesh.verticesCount, mesh.uvsType);

		JsonDeinit(&value);

		package->dependenciesCount = 0;
		package->dependant.resource = manager->AddMesh(mesh);
	}

	ResourceTypeFactoryCreateFunction(MeshManager::CreateMesh) {
		ASSERT(false);
	}

	MeshManager::MeshManager() :
		_allocator(nullptr) {
		static_assert(offsetof(MeshManager, _factoryInterface) == 0, "Factory interface has to be first");
	}

	MeshManager::~MeshManager() {
	}


	void MeshManager::Init(core::IAllocator* allocator) {
		_meshes.Init(allocator, 128);
		_allocator = allocator;
		_factoryInterface.TryCreate = TryCreateMesh;
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

