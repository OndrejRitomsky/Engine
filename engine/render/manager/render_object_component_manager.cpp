#include "render_object_component_manager.h"

#include <core/allocator/allocate.h>
#include <core/common/pointer.h>
#include <core/collection/hashmap.inl>
#include <core/common/utility.h>

#include <core/math/v2.h>
#include <core/math/m44.h>


const u64 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

namespace eng {
	RenderObjectComponentManager::RenderObjectComponentManager() :
		_allocator(nullptr) {

		_data.count = 0;
		_data.capacity = 0;
		_data.data = nullptr;

		_data.ids = nullptr;
		_data.resources = nullptr;
		_data.resourcesReady = nullptr;
		_data.resourcesHash = nullptr;
	}

	RenderObjectComponentManager::~RenderObjectComponentManager() {
		if (_data.data) {
			Deallocate(_allocator, _data.data);
		}
	}

	void RenderObjectComponentManager::Init(core::IAllocator* allocator) {
		ASSERT(!_allocator);

		_allocator = allocator;
		_map.Init(allocator);
	}

	void RenderObjectComponentManager::Reserve(u32 capacity) {
		if (capacity > _data.capacity)
			Reallocate(capacity);
	}

	bool RenderObjectComponentManager::Find(Entity entity, RenderObjectComponent* outComponent) const {
		const RenderObjectComponent* ind = _map.Find(entity.Hash());
		if (ind) {
			outComponent->h = ind->h;
			return true;
		}

		return false;
	}

	void RenderObjectComponentManager::SetEmpty(RenderObjectComponent handle) {
		u64 i = handle.h;
		_data.ids[i] = Entity::MakeInvalid();
		_data.resources[i] = 0;
		_data.resourcesReady[i] = 0;
		_data.resourcesReady[i] = false;
	}

	RenderObjectComponent RenderObjectComponentManager::Create(Entity entity) {
		RenderObjectComponent handle;
		handle.h = 0;
		if (_map.Find(entity.Hash()))
			return handle;

		if (_data.count == _data.capacity)
			Reallocate(core::Max<u32>((_data.capacity * 2), UNINITIALIZED_PUSH_INIT_CAPACITY));

		handle.h = _data.count;
		++_data.count;
		SetEmpty(handle);
		_data.ids[handle.h] = entity;

		_map.Add(entity.Hash(), handle);
		return handle;
	}

	bool RenderObjectComponentManager::Remove(Entity entity) {
		RenderObjectComponent* handle = _map.Find(entity.Hash());
		if (!handle)
			return false;

		u32 last = _data.count - 1;
		u64 i = static_cast<u64>(handle->h);

		if (i < last) {
			RenderObjectComponent* lastHandle = _map.Find(_data.ids[last].Hash());
			ASSERT(lastHandle);

			_data.ids[i] = _data.ids[last];
			_data.resources[i] = _data.resources[last];
			_data.resourcesHash[i] = _data.resourcesHash[last];
			_data.resourcesReady[i] = _data.resourcesReady[last];

			lastHandle->h = i;
		}

		--_data.count;
		return true;
	}

	void RenderObjectComponentManager::Reallocate(u32 capacity) {
		ASSERT(capacity > 0);

		// @TODO macro ? template ? none ? 
		u32 sizeNeeded = capacity * (sizeof(Entity) + sizeof(Resource) + sizeof(bool) + sizeof(h64));
		sizeNeeded += alignof(Resource) + alignof(h64) + alignof(bool);

		Data data;
		data.count = _data.count;
		data.capacity = capacity;
		data.data = Allocate(_allocator, sizeNeeded, alignof(Entity));

		{
			data.ids            = (Entity*) data.data;
			data.resources      = (Resource*) core::PointerAlign(&data.ids[capacity], alignof(Resource));
			data.resourcesHash = (h64*) core::PointerAlign(&data.resources[capacity], alignof(h64));
			data.resourcesReady = (bool*) core::PointerAlign(&data.resourcesHash[capacity], alignof(bool));
		}

		if (_data.data) {
			core::Memcpy(data.ids, _data.ids, data.count * sizeof(Entity));
			core::Memcpy(data.resources, _data.resources, data.count * sizeof(Resource));
			core::Memcpy(data.resourcesHash, _data.resourcesHash, data.count * sizeof(h64));
			core::Memcpy(data.resourcesReady, _data.resourcesReady, data.count * sizeof(bool));

			Deallocate(_allocator, _data.data);
		}

		_data = data;

		if (_data.count == 0) {
			_data.count = 1;
			// @TODO this is not ok ... its not ready after INIT!!!
			SetEmpty({0}); // first is invalid
		}
	}

	bool RenderObjectComponentManager::GetReady(RenderObjectComponent handle) const {
		return _data.resourcesReady[handle.h];
	}

	void RenderObjectComponentManager::SetReady(RenderObjectComponent handle, bool ready) {
		_data.resourcesReady[handle.h] = ready;
	}

	Resource RenderObjectComponentManager::GetResource(RenderObjectComponent handle) const {
		return _data.resources[handle.h];
	}

	void RenderObjectComponentManager::SetResource(RenderObjectComponent handle, Resource resource) {
		_data.resources[handle.h] = resource;
	}

	h64 RenderObjectComponentManager::GetResourceHash(RenderObjectComponent handle) const {
		return _data.resourcesHash[handle.h];
	}

	void RenderObjectComponentManager::SetResourceHash(RenderObjectComponent handle, h64 hash) {
		_data.resourcesHash[handle.h] = hash;
	}

	Entity RenderObjectComponentManager::GetEntity(RenderObjectComponent handle) const {
		return _data.ids[handle.h];
	}

	const RenderObjectComponent* RenderObjectComponentManager::RenderComponents(u32* outCount) const {
		ASSERT2(_map.end() - _map.begin() == _map.Count(), "Map has to have continious layout");
		*outCount = _map.Count();
		return _map.begin();
	}
}
