#pragma once

#include <core/common/types.h>

#include "resource/resource.h"

namespace eng {


	// @TODO ?? SPLIT
	enum class ResourceEventType : u8 {
		FS_LOAD,      // output
		LOADED,       // output
		LOAD_ONE_TYPE,        // input
		PRELOAD,      // input
		DATA_LOADED,  // input
		REGISTER      // input
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

	// Debug one
	struct ResourcePreloadEvents {
		u32 count;
		const u64* typeIDs;
		const h64* hashes;
	};

	// Debug one
	struct ResourceRegisterEvents : ResourceDataLoadedEvents {
	};
}