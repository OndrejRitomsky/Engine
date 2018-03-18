#pragma once

#include <Core/Types.h>

namespace render {
	using SortKey = u64;

	struct CommandProxy {
		SortKey sortKey;
		const char* data;
	};
}