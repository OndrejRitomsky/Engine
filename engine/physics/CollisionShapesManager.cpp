#include "CollisionShapesManager.h"

#include <Core/Allocator/IAllocator.h>
#include <Core/Collection/HashMap.inl>

#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"

namespace eng {

	//---------------------------------------------------------------------------
	CollisionShapesManager::CollisionShapesManager() {

	}

	//---------------------------------------------------------------------------
	CollisionShapesManager::~CollisionShapesManager() {

	}

	//---------------------------------------------------------------------------
	void CollisionShapesManager::Init(Engine* engine, core::IAllocator* allocator) {
		_entityIDToCircleCollider.Init(allocator);
		_AABBcolliders.Init(allocator);
	}

	//---------------------------------------------------------------------------
	bool CollisionShapesManager::AddCircleCollider(EntityID entityID, const CircleCollider& collider) {
		if (_entityIDToCircleCollider.Find(entityID))
			return false;

		_entityIDToCircleCollider.Add(entityID, collider);
		return true;
	}

	//---------------------------------------------------------------------------
	bool CollisionShapesManager::RemoveCircleCollider(EntityID entityID) {
		return _entityIDToCircleCollider.Remove(entityID);
	}

	//---------------------------------------------------------------------------
	CircleCollider* CollisionShapesManager::GetCircleCollider(EntityID entityID) {
		return _entityIDToCircleCollider.Find(entityID);
	}

	//---------------------------------------------------------------------------
	const core::HashMap<CircleCollider>& CollisionShapesManager::GetCircleColliders() {
		return _entityIDToCircleCollider;
	}

	//---------------------------------------------------------------------------
	bool CollisionShapesManager::AddAABBCollider(EntityID entityID, const AABBCollider& collider) {
		if (_AABBcolliders.Find(entityID))
			return false;

		_AABBcolliders.Add(entityID, collider);
		return true;
	}

	//---------------------------------------------------------------------------
	bool CollisionShapesManager::RemoveAABBCollider(EntityID entityID) {
		return _AABBcolliders.Remove(entityID);
	}

	//---------------------------------------------------------------------------
	AABBCollider* CollisionShapesManager::GetAABBCollider(EntityID entityID) {
		return _AABBcolliders.Find(entityID);
	}

	//---------------------------------------------------------------------------
	const core::HashMap<AABBCollider>& CollisionShapesManager::GetAABBColliders() {
		return _AABBcolliders;
	}

}