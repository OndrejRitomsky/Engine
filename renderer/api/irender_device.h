#pragma once

#include <core/common/types.h>

namespace render {
	class RenderContext;
	struct ResourceContext;

	class IRenderDevice {
	public:
		virtual ~IRenderDevice() = default;

		virtual void Render(const RenderContext* rc, u64 count) = 0;
		virtual void ManageResources(const ResourceContext* rrc, u64 count) = 0;
	};
}