#pragma once

#include <core/common/types.h>

#include "../../world/entity.h"
#include "../../resources/resource/resource.h"

namespace eng {
	// wastes 4 bytes currently
	struct RenderEntity {
		Resource renderObject;
		Entity entity;
	};

	struct RenderEntityEvents {
		u32 count;
		const RenderEntity* entities;
	};

	struct RenderLoadRenderObjectEvents {
		u32 count;
		const h64* hashes;
	};
}