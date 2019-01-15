#include "transform2d_component_manager.h"

#include <core/allocator/allocate.h>
#include <core/common/pointer.h>
#include <core/collection/hashmap.inl>
#include <core/common/utility.h>

#include <core/math/v2.h>
#include <core/math/m44.h>

#include "../entity.h"

const u64 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

namespace eng {
	Transform2DComponentManager::Transform2DComponentManager() :
		_allocator(nullptr) {

		_data.count = 0;
		_data.capacity = 0;
		_data.data = nullptr;

		_data.id = nullptr;
		_data.dirty = nullptr;
		_data.angle = nullptr;
		_data.position = nullptr;
		_data.scale = nullptr;
		_data.transform = nullptr;
	}

	Transform2DComponentManager::~Transform2DComponentManager() {
		if (_data.data) {
			Deallocate(_allocator, _data.data);
		}
	}

	void Transform2DComponentManager::Init(core::IAllocator* allocator) {
		ASSERT(!_allocator);

		_allocator = allocator;
		_map.Init(allocator);
	}

	void Transform2DComponentManager::Reserve(u32 capacity) {
		if (capacity > _data.capacity)
			Reallocate(capacity);
	}

	Transform2DComponent Transform2DComponentManager::Find(Entity entity) const {
		const Transform2DComponent* ind = _map.Find(entity.Hash());

		Transform2DComponent handle;
		handle.h = ind ? ind->h : 0;
		return handle;
	}

	void Transform2DComponentManager::SetEmpty(Transform2DComponent handle) {
		u64 i = handle.h;
		_data.id[i] = Entity::MakeInvalid();
		_data.dirty[i] = false;
		_data.angle[i] = 0;
		_data.position[i] = V2(0, 0);
		_data.scale[i] = V2(1, 1);
		_data.transform[i] = MatrixIdentity();
	}

	Transform2DComponent Transform2DComponentManager::Create(Entity entity) {
		Transform2DComponent handle;
		handle.h = 0;
		if (_map.Find(entity.Hash()))
			return handle;

		if (_data.count == _data.capacity)
			Reallocate(core::Max<u32>((_data.capacity * 2), UNINITIALIZED_PUSH_INIT_CAPACITY));

		handle.h = _data.count;
		++_data.count;
		SetEmpty(handle);
		_data.id[handle.h] = entity;
		_map.Add(entity.Hash(), handle);

		return handle;
	}

	bool Transform2DComponentManager::Remove(Entity entity) {
		Transform2DComponent* handle = _map.Find(entity.Hash());
		if (!handle)
			return false;

		u32 last = _data.count - 1;
		u64 i = static_cast<u64>(handle->h);

		if (i < last) {
			Transform2DComponent* lastHandle = _map.Find(_data.id[last].Hash());
			ASSERT(lastHandle);

			_data.id[i] = _data.id[last];
			_data.dirty[i] = _data.dirty[last];
			_data.angle[i] = _data.angle[last];
			_data.position[i] = _data.position[last];
			_data.scale[i] = _data.scale[last];
			_data.transform[i] = _data.transform[last];

			lastHandle->h = i;
		}

		--_data.count;
		return true;
	}

	void Transform2DComponentManager::Reallocate(u32 capacity) {
		ASSERT(capacity > 0);



		// @TODO macro ? template ? none ? 
		u32 sizeNeeded = capacity * (sizeof(Entity) + sizeof(bool) + sizeof(f32) + sizeof(V2) * 2 + sizeof(M44));
		sizeNeeded += alignof(bool) + alignof(f32) + alignof(V2) * 2 + alignof(M44);

		Data data;
		data.count = _data.count;
		data.capacity = capacity;
		data.data = Allocate(_allocator, sizeNeeded, alignof(Entity));

		{
			data.id        = (Entity*) core::PointerAlign(data.data, alignof(Entity));
			data.dirty     = (bool*) core::PointerAlign(&data.id[capacity], alignof(bool));
			data.angle     = (f32*) core::PointerAlign(&data.dirty[capacity], alignof(f32));
			data.position  = (V2*) core::PointerAlign(&data.angle[capacity], alignof(V2));
			data.scale     = (V2*) core::PointerAlign(&data.position[capacity], alignof(V2));
			data.transform = (M44*) core::PointerAlign(&data.scale[capacity], alignof(M44));
		}

		if (_data.data) {
			core::Memcpy(data.id, _data.id, data.count * sizeof(Entity));
			core::Memcpy(data.dirty, _data.dirty, data.count * sizeof(bool));
			core::Memcpy(data.angle, _data.angle, data.count * sizeof(f32));
			core::Memcpy(data.position, _data.position, data.count * sizeof(V2));
			core::Memcpy(data.scale, _data.scale, data.count * sizeof(V2));
			core::Memcpy(data.transform, _data.transform, data.count * sizeof(M44));

			Deallocate(_allocator, _data.data);
		}

		_data = data;

		if (_data.count == 0) {
			_data.count = 1;

			SetEmpty({0}); // first is invalid
		}
	}

	f32 Transform2DComponentManager::Angle(Transform2DComponent handle) const {
		return _data.angle[handle.h];
	}

	void Transform2DComponentManager::SetAngle(Transform2DComponent handle, f32 angle) {
		_data.angle[handle.h] = angle;
		_data.dirty[handle.h] = true;
	}

	const V2&  Transform2DComponentManager::Position(Transform2DComponent handle) const {
		return _data.position[handle.h];
	}

	void Transform2DComponentManager::SetPosition(Transform2DComponent handle, const V2&position) {
		_data.position[handle.h] = position;
		_data.dirty[handle.h] = true;
	}

	const V2& Transform2DComponentManager::Scale(Transform2DComponent handle) const {
		return _data.scale[handle.h];
	}

	void Transform2DComponentManager::SetScale(Transform2DComponent handle, const V2& scale) {
		_data.scale[handle.h] = scale;
		_data.dirty[handle.h] = true;
	}

	const M44& Transform2DComponentManager::Transform(Transform2DComponent handle) const {
		if (_data.dirty[handle.h]) {
			V2 scale = Scale(handle);
			V2 pos = Position(handle);
			_data.transform[handle.h] = MatrixScale(scale.x, scale.y, 0) * MatrixRotateZ(_data.angle[handle.h]) * MatrixTranslate(pos.x, pos.y, 0);
			_data.dirty[handle.h] = false;
		}

		return _data.transform[handle.h];
	}
}
