#pragma once

#include <core/common/types.h>

namespace eng {
	struct RenderObjectComponent;

	enum class RenderEventType : u8 {
		RESOURCES_LOADED
	};

	struct RenderResourcesLoadedEvents {
		u32 count;
		const h64* hashes;
	};

}