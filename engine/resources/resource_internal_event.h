#pragma once

#include <core/common/types.h>

#include "resource.h"


namespace eng {
	// There is no associated ResourceEventType
	// Its only for internal in module data passing

	struct ResourceDependencyLoadEvents {
		u32 count;
		const u64* typesID;
		const h64* hashes;
	};

	struct ResourceReadyEvents {
		u32 count;
		const h64* hashes;
		const Resource* resources;
	};
}