#pragma once


#include <Core/Collection/Array.h>

#include "Engine/Entity/Entity.h"

#include <Math/V2.h>

namespace core {
	class IAllocator;
	class ThreadPool;
}

namespace eng {
	class Engine;
	class RigidBody2DManager;
	class CollisionShapesManager;

	struct Collision {
		EntityID entity;
		EntityID entity2;
		math::V2 normal;
		f32 depth;
	};

	class Physics2D {
	public:
		Physics2D();
		~Physics2D();

		void Init(Engine* engine, core::IAllocator* allocator);

		void Update(f32 delta);

		RigidBody2DManager* GetRigidBody2DManager();
		CollisionShapesManager* GetCollisionShapesManager();
	
	private:
		core::Array<Collision> _collisions;
		core::Array<Collision> _halfCollisions;

		core::IAllocator* _allocator;
		RigidBody2DManager* _rigidBodyManager;
		CollisionShapesManager* _collisionShapesManager;

		class DebugRenderer* _debugRenderer;
	};
}