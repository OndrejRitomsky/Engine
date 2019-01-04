#pragma once

#include <core/common/types.h>

namespace eng {
	struct RenderObjectComponent;

	enum class RenderEventType : u8 {
		LOAD_RENDER_OBJECT,
		RENDER // input
	};

	struct RenderRenderEvents {
		u32 count;
		const RenderObjectComponent* renderComponents;
	};

	struct RenderLoadRenderObject {
		u32 count;
		const h64* hashes;
	};
}