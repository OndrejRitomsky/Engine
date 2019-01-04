#include "Physics2D.h"

#include <Core/Utility.h>
#include <Core/Collection/Array.inl>
#include <Core/Allocator/IAllocator.h>
#include <Core/Collection/HashMap.inl>


#include "Engine/Physic/RigidBody2DManager.h"
#include "Engine/Physic/CollisionShapesManager.h"

#include "Engine/Renderer/DebugRenderer.h"

#include "Engine/Engine.h"

namespace eng {
	//---------------------------------------------------------------------------
	Physics2D::Physics2D() :
		_rigidBodyManager(nullptr),
		_collisionShapesManager(nullptr) {

	}

	//---------------------------------------------------------------------------
	Physics2D::~Physics2D() {
		if (_rigidBodyManager)
			_allocator->Destroy(_rigidBodyManager);

		if (_collisionShapesManager)
			_allocator->Destroy(_collisionShapesManager);
	}

	//---------------------------------------------------------------------------
	void Physics2D::Init(Engine* engine, core::IAllocator* allocator) {
		_allocator = allocator;
		_rigidBodyManager = _allocator->Make<RigidBody2DManager>();
		_collisionShapesManager = _allocator->Make<CollisionShapesManager>();

		_rigidBodyManager->Init(engine, allocator);
		_collisionShapesManager->Init(engine, allocator);
		_debugRenderer = engine->GetDebugRenderer();
		_collisions.Init(allocator);
		_halfCollisions.Init(allocator);
	}

	//---------------------------------------------------------------------------
	static f32 HitCoeff(math::V2 v1, math::V2 v2, math::V2 normal, f32 w1, f32 w2) {

		math::V2 velDiff = v2 - v1;
		velDiff.x *= normal.x;
		velDiff.y *= normal.y * w1 * w2;

		return (-2.f / (w1 + w2)) * (velDiff.x + velDiff.y);
	}

	//---------------------------------------------------------------------------
	static bool CircleCollision(const math::V2& pos1, f32 radius1, const math::V2& pos2, f32 radius2) {
		math::V2 vec = pos2 - pos1;
		f32 sqRadius = (radius1 + radius2) * (radius1 + radius2);
		return sqRadius - vec.MagnitudeSqr() > 0;
	}

	//---------------------------------------------------------------------------
	static bool CircleAABBCollision(math::V2& tl, f32 width, f32 height, const math::V2& circlePos, f32 radius) {
		math::V2 closest;
		closest.x = core::Clamp(circlePos.x, tl.x, tl.x + width);
		closest.y = core::Clamp(circlePos.y, tl.y, tl.y + height);

		math::V2 vec = circlePos - closest;
		f32 sqRadius = radius * radius;

		tl = closest;

		return sqRadius - vec.MagnitudeSqr() > 0;
	}

	//---------------------------------------------------------------------------
	void Physics2D::Update(f32 delta) {
		_rigidBodyManager->Update(delta);

		_collisions.Clear();
		_halfCollisions.Clear();



		const core::HashMap<RigidBody2D>& rigidBodies = _rigidBodyManager->GetRigidBodies(false);
		const core::HashMap<RigidBody2D>& staticRigidBodies = _rigidBodyManager->GetRigidBodies(true);

		for (auto it = rigidBodies.cKeyValueBegin(); it != rigidBodies.cKeyValueEnd(); ++it) {
			const RigidBody2D& rigidBody = it.value;
			EntityID id = it.key;

			CircleCollider* circle = _collisionShapesManager->GetCircleCollider(id);
			
			if (!circle) 
				continue;

			math::V2 position = rigidBody.current.transform.position + circle->offset;

			for (auto it2 = staticRigidBodies.cKeyValueBegin(); it2 != staticRigidBodies.cKeyValueEnd(); ++it2) {
				EntityID id2 = it2.key;
				AABBCollider* aabb = _collisionShapesManager->GetAABBCollider(id2);

				if (!aabb)
					continue;

				math::V2 position2 = it2.value.current.transform.position + aabb->offset;
				position2.x -= aabb->width / 2.f;
				position2.y -= aabb->height / 2.f;
				if (CircleAABBCollision(position2, aabb->width, aabb->height, position, circle->radius)) {
					position2 += math::V2(0.00001f);
					math::V2 vec = position - position2;
					Collision col;
					col.entity = id;
					col.entity2 = id2;
					col.normal = vec.Normalized();
					col.depth = circle->radius - vec.Magnitude();
					_halfCollisions.Push(core::move(col));
				}
			}

			for (auto it2 = it + 1; it2 != rigidBodies.cKeyValueEnd(); ++it2) {
				EntityID id2 = it2.key;
				CircleCollider* circle2 = _collisionShapesManager->GetCircleCollider(id2);

				if (!circle2)
					continue;

				math::V2 position2 = it2.value.current.transform.position + circle2->offset;

				if (CircleCollision(position, circle->radius, position2, circle2->radius)) {
					position2 += math::V2(0.00001f);
					math::V2 vec = position2 - position;
					Collision col;
					col.entity = id;
					col.entity2 = id2;
					col.normal = vec.Normalized();
					col.depth = circle->radius + circle2->radius - vec.Magnitude();
					_collisions.Push(core::move(col));
				}
			}
		}

		// could be offset
		for (Collision& col : _collisions) {
			RigidBody2D* rs1 = _rigidBodyManager->GetRigidBody2D(col.entity, false);
			RigidBody2D* rs2 = _rigidBodyManager->GetRigidBody2D(col.entity2, false);

			f32 k = HitCoeff(rs1->current.velocity, rs2->current.velocity, col.normal, rs1->mass, rs2->mass);

			rs1->current.transform.position -= col.normal * col.depth / 2.f;
			rs2->current.transform.position += col.normal * col.depth / 2.f;

			rs1->current.velocity -= k * col.normal / rs1->mass;
			rs2->current.velocity += k * col.normal / rs2->mass;
		}

		// could be offset
		for (Collision& col : _halfCollisions) {
			RigidBody2D* rs1 = _rigidBodyManager->GetRigidBody2D(col.entity, false);

			rs1->current.velocity = rs1->current.velocity.Reflect(col.normal);
			rs1->current.transform.position += col.normal * col.depth;
			
		}
	}

	//---------------------------------------------------------------------------
	RigidBody2DManager* Physics2D::GetRigidBody2DManager() {
		return _rigidBodyManager;
	}

	//---------------------------------------------------------------------------
	CollisionShapesManager* Physics2D::GetCollisionShapesManager() {
		return _collisionShapesManager;
	}

}