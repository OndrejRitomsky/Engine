#pragma once

#include <core/common/types.h>

namespace eng {

	enum class FileSystemEventType : u8 {
		REGISTER_PATH,
		LOAD,
		LOADED
	};

	// @TODO SOA

	struct FileSystemRegisterPathEvents {
		u32 count;
		h64* hashes;
		const char** paths;
	};

	struct FileSystemLoadEvents {
		u32 count;
		const h64* hashes;
		const u64* extraData;
	};

	// @TODO soa
	struct FileSystemEventLoad {
		h64 nameHash;
		u64 extraData;
	};

	struct FileSystemLoadedEvents {
		u32 count;
		h64* nameHashes;
		u64* extraData;
		u64* bufferSizes;
		char** buffers;
	};

	// @CLEANUP
	// The extra data is just some info that the request should carry, because it will be needed when the request is finished
	// (for resources its ex. resource type, there could cleaner solution)
	// (one is take the information from resource loader, but thats extra look up and extra api function)

}