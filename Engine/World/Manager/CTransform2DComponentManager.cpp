#include "CTransform2DComponentManager.h"

#include <Core/Allocator/IAllocator.h>
#include <Core/Common/Pointer.h>
#include <Core/Collection/HashMap.inl>
#include <Core/Common/Utility.h>

#include <Math/V2.h>
#include <Math/M44.h>

const u64 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

namespace eng {

	//---------------------------------------------------------------------------
	CTransform2DComponentManager::CTransform2DComponentManager() :
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

	//---------------------------------------------------------------------------
	CTransform2DComponentManager::~CTransform2DComponentManager() {
		if (_data.data) {
			_allocator->Deallocate(_data.data);
		}
	}

	//---------------------------------------------------------------------------
	void CTransform2DComponentManager::Init(core::IAllocator* allocator) {
		ASSERT(!_allocator);

		_allocator = allocator;
		_map.Init(allocator);
	}

	//---------------------------------------------------------------------------
	void CTransform2DComponentManager::Reserve(u32 capacity) {
		if (capacity > _data.capacity)
			Reallocate(capacity);
	}

	//---------------------------------------------------------------------------
	Transform2DComponent CTransform2DComponentManager::Find(Entity entity) const {
		const Transform2DComponent* ind = _map.Find(entity.h);

		Transform2DComponent handle;
		handle.h = ind ? ind->h : 0;
		return handle;
	}

	//---------------------------------------------------------------------------
	void CTransform2DComponentManager::SetEmpty(Transform2DComponent handle) {
		u64 i = handle.h;
		_data.id[i] = Entity::MakeInvalid();
		_data.dirty[i] = false;
		_data.angle[i] = 0;
		_data.position[i] = math::V2(0, 0);
		_data.scale[i] = math::V2(1, 1);
		_data.transform[i] = math::IdentityMatrix();
	}

	//---------------------------------------------------------------------------
	Transform2DComponent CTransform2DComponentManager::Create(Entity entity) {
		Transform2DComponent handle;
		handle.h = 0;
		if (_map.Find(entity.h))
			return handle;

		if (_data.count == _data.capacity)
			Reallocate(core::Max<u32>((_data.capacity * 2), UNINITIALIZED_PUSH_INIT_CAPACITY));

		handle.h = _data.count;
		++_data.count;
		SetEmpty(handle);
		_map.Add(entity.h, handle);

		return handle;
	}

	//---------------------------------------------------------------------------
	bool CTransform2DComponentManager::Remove(Entity entity) {
		Transform2DComponent* handle = _map.Find(entity.h);
		if (!handle)
			return false;

		u32 last = _data.count - 1;
		u64 i = static_cast<u64>(handle->h);

		bool found = _map.Find(_data.id[last].h);
		ASSERT(found);

		_data.id[i] = _data.id[last];
		_data.dirty[i] = _data.dirty[last];
		_data.angle[i] = _data.angle[last];
		_data.position[i] = _data.position[last];
		_data.scale[i] = _data.scale[last];
		_data.transform[i] = _data.transform[last];

		--_data.count;
		return true;
	}

	//---------------------------------------------------------------------------
	void CTransform2DComponentManager::Reallocate(u32 capacity) {
		ASSERT(capacity > 0);

		using namespace math;

		// @TODO macro ? template ? none ? 
		u32 sizeNeeded = capacity * (sizeof(Entity) + sizeof(bool) + sizeof(f32) + sizeof(V2) * 2 + sizeof(M44));
		sizeNeeded += alignof(bool) + alignof(f32) +alignof(V2) * 2 + alignof(M44);

		Data data;
		data.count = _data.count;
		data.capacity = capacity;
		data.data = _allocator->Allocate(sizeNeeded, alignof(Entity));

		{
			using namespace core::mem;
			data.id = Align<Entity>(data.data);
			data.dirty = Align<bool>(&data.id[capacity]);
			data.angle = Align<f32>(&data.dirty[capacity]);
			data.position = Align<V2>(&data.angle[capacity]);
			data.scale = Align<V2>(&data.position[capacity]);
			data.transform = Align<M44>(&data.scale[capacity]);
		}

		if (_data.data) {
			Memcpy(data.id, _data.id, data.count * sizeof(Entity));
			Memcpy(data.dirty, _data.dirty, data.count * sizeof(bool));
			Memcpy(data.angle, _data.angle, data.count * sizeof(f32));
			Memcpy(data.position, _data.position, data.count * sizeof(V2));
			Memcpy(data.scale, _data.scale, data.count * sizeof(V2));
			Memcpy(data.transform, _data.transform, data.count * sizeof(M44));

			_allocator->Deallocate(_data.data);
		}

		_data = data;

		if (_data.count == 0) {
			_data.count = 1;

			SetEmpty({0}); // first is invalid
		}
	}

	//---------------------------------------------------------------------------
	f32 CTransform2DComponentManager::Angle(Transform2DComponent handle) const {
		return _data.angle[handle.h];
	}

	//---------------------------------------------------------------------------
	void CTransform2DComponentManager::SetAngle(Transform2DComponent handle, f32 angle) {
		_data.angle[handle.h] = angle;
		_data.dirty[handle.h] = true;
	}

	//---------------------------------------------------------------------------
	const math::V2&  CTransform2DComponentManager::Position(Transform2DComponent handle) const {
		return _data.position[handle.h];
	}

	//---------------------------------------------------------------------------
	void CTransform2DComponentManager::SetPosition(Transform2DComponent handle, const math::V2&position) {
		_data.position[handle.h] = position;
		_data.dirty[handle.h] = true;
	}

	//---------------------------------------------------------------------------
	const math::V2& CTransform2DComponentManager::Scale(Transform2DComponent handle) const {
		return _data.scale[handle.h];
	}

	//---------------------------------------------------------------------------
	void CTransform2DComponentManager::SetScale(Transform2DComponent handle, const math::V2& scale) {
		_data.scale[handle.h] = scale;
		_data.dirty[handle.h] = true;
	}

	//---------------------------------------------------------------------------
	const math::M44& CTransform2DComponentManager::Transform(Transform2DComponent handle) const {
		if (_data.dirty[handle.h]) {
			math::V2 scale = Scale(handle);
			math::V2 pos = Position(handle);
			_data.transform[handle.h] = math::ScaleMatrix(scale.x, scale.y, 0) * math::RotateZMatrix(_data.angle[handle.h]) * math::TranslateMatrix(pos.x, pos.y, 0);
			_data.dirty[handle.h] = false;
		}

		return _data.transform[handle.h];
	}
}
