#pragma once

#include <core/common/types.h>
#include <core/collection/array.h>

namespace core {
	struct IAllocator;
}

namespace eng {
	class CStringHashBank;

	class FileWatcher {
	public:
		FileWatcher(core::IAllocator* allocator, CStringHashBank* pathBank);
		~FileWatcher() = default;

		// Registers path to watch for changes
		bool WatchFile(h64 hash);

		// Checks count files, returns if finds changed;
		bool CheckNext(u32 count, h64& outHash);

	private:

		u64 CheckTime(h64 hash) const;

	private:
		u32 _index;
		core::Array<h64> _hashes;
		core::Array<u64> _times;

		CStringHashBank* _pathBank;
	};
}
