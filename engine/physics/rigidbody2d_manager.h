#pragma once

#include <Core/CoreTypes.h>
#include <Core/Collection/Array.h>
#include <Core/Collection/HashMap.h>

#include "Engine/Physic/RigidBody2D.h"

#include "Engine/Entity/Transform2D.h"
#include "Engine/Entity/Entity.h"


namespace core {
	class IAllocator;
	class JobPool;
}

namespace eng {

	class Engine;

	class RigidBody2DManager {
	public:
		RigidBody2DManager();
		~RigidBody2DManager();

		void Init(Engine* engine, core::IAllocator* allocator);

		void Update(f32 delta);

		void SetTransformsForIDs(const Transform2D* transforms, const EntityID* indexToEntity, u32 count);

		const core::HashMap<RigidBody2D>& GetRigidBodies(bool isStatic);

		bool AddRigidBody2D(EntityID entityID, const RigidBody2D& rigidBody, bool isStatic);

		RigidBody2D* RigidBody2DManager::GetRigidBody2D(EntityID entityID, bool isStatic);

		bool RemoveRigidBody2D(EntityID entityID, bool isStatic);

	private:
		core::JobPool* _threadPool;
		// @TODO CHANGE???
		core::HashMap<RigidBody2D> _staticRigidBodies;
		core::HashMap<RigidBody2D> _dynamicRigidBodies;
	};
};

