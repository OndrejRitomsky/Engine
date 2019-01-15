#pragma once

#include <core/common/types.h>

#include "command_proxy.h"
#include "../render_resources.h"


namespace render {
	enum class BatchType : u8 {
		LINES = 0,
		LINE_LOOP,
		TRIANGLES
	};

	struct BatchDescription {
		BatchType type;

	  IndexType indexType; // only used if indices are used

		u32 instances;
		u32 vertices;
		u32 indices;

		u32 vertexOffset; // only used if indices are not used
	};

	
	// This acts as header to data stream
	struct JobPackage {
		BatchDescription batch;
		Resource shader;

		u32 size;            // All included sizeof package + data after 

		// @TODO Or data pointer?
		u32 resourcesOffset; // From start of this struct -> (RenderResource*) 
		u32 resourcesCount;

		u32 uniformsOffset; // Uniforms dont have uniform data length
		u32 uniformsCount;

		SortKey sortKey;
	};
}