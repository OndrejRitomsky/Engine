#pragma once

#include <Core/Common/Types.h>

namespace render {
	class RenderContext;
	class RenderResourceContext;

	class IRenderDevice {
	public:
		virtual ~IRenderDevice() = default;

		virtual void Render(const RenderContext* rc, u64 count) = 0;
		virtual void ManageResources(const RenderResourceContext* rrc, u64 count) = 0;
	};
}