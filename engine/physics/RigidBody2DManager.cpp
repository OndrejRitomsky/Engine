#include "RigidBody2DManager.h"

#include <Core/Allocator/IAllocator.h>
#include <Core/Utility.h>
#include <Core/Collection/HashMap.inl>

#include "Engine/Engine.h"

#include "Engine/Scene/Scene.h"

#include "memory.h"

namespace eng {

	//---------------------------------------------------------------------------
	RigidBody2DManager::RigidBody2DManager() :
		_threadPool(nullptr) {

	}

	//---------------------------------------------------------------------------
	RigidBody2DManager::~RigidBody2DManager() {

	}

	//---------------------------------------------------------------------------
	void RigidBody2DManager::Init(Engine* engine, core::IAllocator* allocator) {
		_staticRigidBodies.Init(allocator);
		_dynamicRigidBodies.Init(allocator);
		_threadPool = engine->GetThreadPool();
	}

	//---------------------------------------------------------------------------
	void RigidBody2DManager::SetTransformsForIDs(const Transform2D *transforms, const EntityID* indexToEntity, u32 count) {
	/*	for (u32 i = 0; i < count; ++i) {
			EntityID id = indexToEntity[i];

			u32 index;
			RigidBody2D* rigidBody = GetRigidBody2D(id, false);
			if (rigidBody)
				memcpy(&rigidBody->transform, &transforms[i], sizeof(Transform2D));
			// If the entity wasnt found, it should be ok, this update might be delayed. (or is it?)
		}*/
	}

	//---------------------------------------------------------------------------
	const core::HashMap<RigidBody2D>& RigidBody2DManager::GetRigidBodies(bool isStatic) {	
		return isStatic ? _staticRigidBodies : _dynamicRigidBodies;
	}

	//---------------------------------------------------------------------------
	void RigidBody2DManager::Update(f32 delta) {
		for (auto& rigidBody : _dynamicRigidBodies) {
			memcpy(&rigidBody.old, &rigidBody.current, sizeof(RigidBody2D::State));

			rigidBody.current.velocity += delta * rigidBody.current.acceleration / rigidBody.mass;
			rigidBody.current.transform.position += delta * rigidBody.current.velocity;
		}
	}

	//---------------------------------------------------------------------------
	bool RigidBody2DManager::AddRigidBody2D(EntityID entityID, const RigidBody2D& rigidBody, bool isStatic) {
		auto& rigidBodies = isStatic ? _staticRigidBodies : _dynamicRigidBodies;

		if (rigidBodies.Find(entityID))
			return false;

		RigidBody2D rigid = rigidBody;
		rigid.id = entityID;
		rigidBodies.Add(entityID, core::move(rigid));
		return true;
	}

	//---------------------------------------------------------------------------
	RigidBody2D* RigidBody2DManager::GetRigidBody2D(EntityID entityID, bool isStatic) {
		RigidBody2D* res= _staticRigidBodies.Find(entityID);
		if (res)
			return res;

		return _dynamicRigidBodies.Find(entityID);
	}

	//---------------------------------------------------------------------------
	bool RigidBody2DManager::RemoveRigidBody2D(EntityID entityID, bool isStatic) {
		auto& rigidBodies = isStatic ? _staticRigidBodies : _dynamicRigidBodies;
		return rigidBodies.Remove(entityID);
	}
}