#pragma once

#include <Core/Common/Types.h>

#include "Renderer/API/Command/CommandParameters.h"
#include "Renderer/API/Command/JobPackage.h"

#include "Renderer/API/Resources/Resource.h"


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