#pragma once

#include <core/common/types.h>

#include "../command/command_parameters.h"
#include "../command/job_package.h"

#include "../render_resources.h"


namespace render {

	enum class CommandType : u8 {
		RENDER,
		CLEAR,
		VIEWPORT,
	};

	struct CommandHeader {
		CommandType commandType;
	};

	struct ClearCommand : CommandHeader {
		ClearFlags flags;
	};

	struct ViewPortCommand : CommandHeader {
		i32 width;
		i32 height;
	};


	struct RenderJobCommand : CommandHeader {
		JobPackage jobPackage;
	};
}