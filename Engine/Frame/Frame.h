#pragma once

#include <Core/Common/Types.h>
#include <Core/TagAllocator/MemTag.h>

namespace eng {

	struct Frame {
		core::MemTag gameTag;
		core::MemTag renderPipelineTag;
		core::MemTag rendererTag;

		double gameDelta;
		double realDelta;

		u64 frameIndex; 
	};

}
