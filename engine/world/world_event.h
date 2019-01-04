#pragma once

#include <core/common/types.h>

namespace eng {
	struct RenderObjectComponent;

	// @TODO ?? SPLIT
	enum class WorldEventType : u8 {
		RENDER // output
	};

	// OUTPUT, Request for resource data
	struct WorldRenderEvents {
		u32 count;
		const RenderObjectComponent* renderComponents;
	};

}