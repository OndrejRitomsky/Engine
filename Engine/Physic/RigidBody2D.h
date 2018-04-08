#pragma once

#include <Core/CoreTypes.h>
#include <Core/Collection/Array.h>
#include <Core/Collection/HashMap.h>

#include <Math/V2.h>

#include "Engine/Entity/Transform2D.h"
#include "Engine/Entity/Entity.h"

namespace eng {

	struct RigidBody2D {
		struct State {
			math::V2  velocity;
			math::V2  acceleration;
			Transform2D transform;
		};

		EntityID id;
		f32   mass;
		State old;
		State current;
	};
}