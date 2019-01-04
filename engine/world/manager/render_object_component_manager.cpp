#include "render_object_component_manager.h"

#include <core/allocator/allocate.h>
#include <core/common/pointer.h>
#include <core/collection/hashmap.inl>
#include <core/common/utility.h>

#include <core/math/v2.h>
#include <core/math/m44.h>

#include "../entity.h"

const u64 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

namespace eng {
	RenderObjectComponentManager::RenderObjectComponentManager() :
		_allocator(nullptr) {

		_data.count = 0;
		_data.capacity = 0;
		_data.data = nullptr;

		_data.ids = nullptr;
		_data.resourceHash = nullptr;
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

	RenderObjectComponent RenderObjectComponentManager::Find(const Entity* entity) const {
		const RenderObjectComponent* ind = _map.Find(entity->h);

		RenderObjectComponent handle;
		handle.h = ind ? ind->h : 0;
		return handle;
	}

	void RenderObjectComponentManager::SetEmpty(RenderObjectComponent handle) {
		u64 i = handle.h;
		_data.ids[i] = Entity::MakeInvalid();
		_data.resourceHash[i] = 0;
	}

	RenderObjectComponent RenderObjectComponentManager::Create(Entity* entity) {
		RenderObjectComponent handle;
		handle.h = 0;
		if (_map.Find(entity->h))
			return handle;

		if (_data.count == _data.capacity)
			Reallocate(core::Max<u32>((_data.capacity * 2), UNINITIALIZED_PUSH_INIT_CAPACITY));

		handle.h = _data.count;
		++_data.count;
		SetEmpty(handle);
		_map.Add(entity->h, handle);

		return handle;
	}

	bool RenderObjectComponentManager::Remove(Entity* entity) {
		RenderObjectComponent* handle = _map.Find(entity->h);
		if (!handle)
			return false;

		u32 last = _data.count - 1;
		u64 i = static_cast<u64>(handle->h);

		if (i < last) {
			RenderObjectComponent* lastHandle = _map.Find(_data.ids[last].h);
			ASSERT(lastHandle);

			_data.ids[i] = _data.ids[last];
			_data.resourceHash[i] = _data.resourceHash[last];

			lastHandle->h = i;
		}

		--_data.count;
		return true;
	}

	void RenderObjectComponentManager::Reallocate(u32 capacity) {
		ASSERT(capacity > 0);

		// @TODO macro ? template ? none ? 
		u32 sizeNeeded = capacity * (sizeof(Entity) + sizeof(h64));
		sizeNeeded += alignof(h64);

		Data data;
		data.count = _data.count;
		data.capacity = capacity;
		data.data = Allocate(_allocator, sizeNeeded, alignof(Entity));

		{
			data.ids = (Entity*) data.data;
			data.resourceHash = (h64*) core::PointerAlign(&data.ids[capacity], alignof(h64));
		}

		if (_data.data) {
			core::Memcpy(data.ids, _data.ids, data.count * sizeof(Entity));
			core::Memcpy(data.resourceHash, _data.resourceHash, data.count * sizeof(h64));

			Deallocate(_allocator, _data.data);
		}

		_data = data;

		if (_data.count == 0) {
			_data.count = 1;
			// @TODO this is not ok ... its not ready after INIT!!!
			SetEmpty({0}); // first is invalid
		}
	}

	h64 RenderObjectComponentManager::ResourceHash(RenderObjectComponent handle) const {
		return _data.resourceHash[handle.h];
	}

	void RenderObjectComponentManager::SetResourceHash(RenderObjectComponent handle, h64 hash) {
		_data.resourceHash[handle.h] = hash;
	}

	void RenderObjectComponentManager::GetEntity(RenderObjectComponent handle, Entity* outEntity) const {
		*outEntity = _data.ids[handle.h];
	}

	const RenderObjectComponent* RenderObjectComponentManager::RenderComponents(u32* outCount) const {
		ASSERT2(_map.end() - _map.begin() == _map.Count(), "Map has to have continious laout");
		*outCount = _map.Count();
		return _map.begin();
	}
}
