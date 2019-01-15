#include "render_object_manager.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/lookuparray.inl>
#include <core/algorithm/hash.h>
#include <core/algorithm/cstring.h>
#include <core/toolbox/json.h>

#include "../resource_event.h"
#include "../resource_util.h"
#include "../resources.h"


#include <stddef.h>


namespace eng {
	static h64 JsonStringToHash(JsonValue* value, const char* key) {
		auto kv = JsonObjectFind(value, key);
		if (!kv || !JsonIsString(&kv->value)) {
			ASSERT(false);
		}

		const char* str = JsonGetString(&kv->value);
		return core::Hash(str, core::Strlen(str));
	}


	ResourceTypeFactoryParseFunction(RenderObjectManager::TryCreateRenderObject) {
		ASSERT(ResourcePackage::DEPENDENCIES_CAPACITY >= 2);

		char* text = (char*) resourceDescription;

		JsonValue value;
		if (!JsonParse(text, nullptr, &value)) {
			ASSERT(false);
		}

		if (!JsonIsObject(&value)) {
			ASSERT(false);
		}

		h64 path = JsonStringToHash(&value, "path");
		h64 mesh = JsonStringToHash(&value, "mesh");
		h64 material = JsonStringToHash(&value, "material");

		JsonDeinit(&value);

		ASSERT(path == package->dependant.hash);

		package->dependenciesCount = 2;
		package->dependencies[0].hash = mesh;
		package->dependencies[1].hash = material;

		package->dependencies[0].typeID = (u64) ResourceType::MESH;
		package->dependencies[1].typeID = (u64) ResourceType::MATERIAL;
	}

	ResourceTypeFactoryCreateFunction(RenderObjectManager::CreateRenderObject) {
		RenderObjectManager* manager = (RenderObjectManager*) factory;
		ASSERT(package->dependenciesCount == 2);

		RenderObject ro;
		ro.mesh     = package->dependencies[0].resource;
		ro.material = package->dependencies[1].resource;

		package->dependant.resource = manager->AddRenderObject(ro);
	}


	RenderObjectManager::RenderObjectManager() {
		static_assert(offsetof(RenderObjectManager, _factoryInterface) == 0, "Factory interface has to be first");
	}

	RenderObjectManager::~RenderObjectManager() {}

	void RenderObjectManager::Init(core::IAllocator* allocator) {
		_renderObjects.Init(allocator, 128);
		_factoryInterface.TryCreate = TryCreateRenderObject;
		_factoryInterface.Create = CreateRenderObject;
	}

	IResourceTypeFactory* RenderObjectManager::Factory() {
		return &_factoryInterface;
	}

	void RenderObjectManager::RemoveRenderObject(const Resource& resource) {
		_renderObjects.Remove(ResourceToHandle(resource));
	}

	const RenderObject& RenderObjectManager::GetRenderObject(const Resource& resource) const {
		return _renderObjects.Get(ResourceToHandle(resource));
	}

	Resource RenderObjectManager::AddRenderObject(const RenderObject& program) {
		return HandleToResource(_renderObjects.Add(program));
	}
}

