#pragma once

#include <Core/Common/Types.h>

namespace eng {
	enum class ModuleState : u8 {
		CREATED,
		UNINITIALIZED,
		OK,
		ERROR,
	};
}