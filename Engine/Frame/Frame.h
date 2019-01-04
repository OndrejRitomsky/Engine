#pragma once

#include <core/common/types.h>

namespace core {
	struct ITagAllocator;
}

namespace eng {

	struct Frame {
		core::MemTag gameTag;
		core::MemTag renderPipelineTag;
		core::MemTag rendererTag;

		double gameDelta;
		double realDelta;

		u64 frameIndex; 

		core::ITagAllocator* tagAllocator; // This will be used in non const ways even though Frame will be const
	};

}
