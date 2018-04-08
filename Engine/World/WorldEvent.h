#pragma once

#include <Core/Common/Types.h>
#include "Engine/Resources/Resource/Resource.h"

namespace eng {

	// @TODO SPLIT
	enum class WorldEventType : u8 {

	};

	// Matches FS 
/*	struct ResourceEventLoadFinished {
		u64 bufferSize;
		h64 nameHash;
		char* buffer;
	};

	struct ResourceEventLoad {
		h64 nameHash;
	};

	struct ResourceEventPreloadResource {
		ResourceType type;
		h64 nameHash;
	};

	struct ResourceEventRegisterResource {
		ResourceType type;
		const void* resource; // reinterpret to correct type and copy (do not store)
		h64 nameHash; // its not inside resource for now
	};*/
}