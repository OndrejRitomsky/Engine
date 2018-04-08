#pragma once

#include <Core/Common/Types.h>

#include "Engine/Resources/Resource/Resource.h"

namespace eng {


	// @TODO ?? all the 1 type 8 hash could be SOA
	// Local
	struct ResourceDependencyEvent {
		u64 typeID;
		h64 hash;
		Resource resource;
	};


	// @TODO ?? SPLIT
	enum class ResourceEventType : u8 {
		LOAD,         // output
		LOADED,       // output
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