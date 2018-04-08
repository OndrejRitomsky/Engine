#pragma once

#include <Core/Common/Types.h>

namespace render {
	using SortKey = u64;

	struct CommandProxy {
		SortKey sortKey;
		const char* data;
	};
}