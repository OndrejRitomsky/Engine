#pragma once

#include <core/common/types.h>

#include "../command/command_parameters.h"
#include "../command/job_package.h"

#include "../render_resources.h"


namespace render {
	enum class CommandType : u8 {
		RENDER,
		CLEAR,

		RESOURCE_ALLOCATION, 
		RESOURCE_DEALLOCATION
	};

	struct CommandHeader {
		CommandType commandType;
	};

	struct ClearCommand : CommandHeader {
		ClearFlags flags;
	};

	struct RenderJobCommand : CommandHeader {
		JobPackage jobPackage;
	};

	// In data stream resource can be casted to subtype
	struct AllocationCommand : CommandHeader {
		Resource resource;
	};

	// In data stream  Resource CANNOT BE casted to subtype
	struct DeallocationCommand : CommandHeader {
		Resource resource;
	};

}