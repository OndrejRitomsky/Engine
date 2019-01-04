#pragma once

#include <core/common/types.h>

namespace render {
	using SortKey = u64;

	struct CommandProxy {
		SortKey sortKey;
		const char* data;
	};
}