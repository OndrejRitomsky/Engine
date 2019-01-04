#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/Array.h>
#include <Core/Collection/HashMap.h>

#include "Engine/Physic/Collider2D.h"

#include "Engine/World/Entity.h"


namespace core {
	class IAllocator;
	class JobPool;
}

namespace eng {

	class Engine;

	class CollisionShapesManager {
	public:
		CollisionShapesManager();
		~CollisionShapesManager();

		void Init(Engine* engine, core::IAllocator* allocator);

		bool AddCircleCollider(Entity entityID, const CircleCollider& collider);
		bool RemoveCircleCollider(Entity entityID);
		CircleCollider* GetCircleCollider(Entity entityID);
		const core::HashMap<CircleCollider>& GetCircleColliders();

		bool AddAABBCollider(Entity entityID, const AABBCollider& collider);
		bool RemoveAABBCollider(Entity entityID);
		AABBCollider* GetAABBCollider(Entity entityID);
		const core::HashMap<AABBCollider>& GetAABBColliders();

		/*void Update(f32 delta);
		void SetTransformsForIDs(Transform2D* transforms, EntityID* indexToEntity, u32 count);
		RigidBodies2D* GetRigidBodies();*/

	private:
		//core::JobPool* _threadPool;
		core::HashMap<CircleCollider> _entityIDToCircleCollider;
		core::HashMap<AABBCollider> _AABBcolliders;
	};
};

