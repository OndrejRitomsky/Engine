#pragma once

#include <Core/CoreTypes.h>

#include <Math/V2.h>

namespace eng {

	struct CircleCollider {
		f32 radius;
		math::V2 offset;
	};

	struct AABBCollider {
		f32 width;
		f32 height;
		math::V2 offset;
	};
}