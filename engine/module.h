#pragma once

#include <core/common/types.h>

namespace eng {
	enum class ModuleState : u8 {
		CREATED,
		UNINITIALIZED,
		OK,
		ERROR,
	};
}