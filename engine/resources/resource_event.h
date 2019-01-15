#pragma once

#include <core/common/types.h>

#include "resource.h"

namespace eng {
	// @TODO can we get rid off typeid from loading?


	// @TODO ?? SPLIT
	enum class ResourceEventType : u8 {
		FS_LOAD,      // output
		LOADED,       // output
		LOAD_ONE_TYPE,        // input
		DATA_LOADED,  // input
	};


	// OUTPUT, Request for resource data
	struct ResourceLoadEvents {
		u32 count;
		const h64* hashes;
		const u64* typeIDs;
	};

	// OUTPUT, Resource loaded 
	struct ResourceLoadedEvents {
		u32 count;
		const h64* hashes;
	};

	// INPUT, New resource data coming to the module
	struct ResourceDataLoadedEvents {
		u32 count;
		const u64* bufferSizes;
		const h64* hashes;
		const u64* typeIDs;              // This is a little hacky, type was sent as extra data to file system
		char** buffers;
	};


	struct ResourceLoadOneTypeEvents {
		u32 count;
		u64 typeID;
		const h64* hashes;
	};
}