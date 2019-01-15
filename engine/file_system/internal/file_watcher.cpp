#include "file_watcher.h"

#include <core/common/Utility.h>
#include <core/collection/array.inl>
#include <core/allocator/allocate.h>
#include <core/platform/file_system.h>

#include "../manager/string_hashbank.h"

namespace eng {
	FileWatcher::FileWatcher(core::IAllocator* allocator, StringHashBank* pathBank) :
		_pathBank(pathBank) {

		_hashes.Init(allocator);
		_times.Init(allocator);
	}

	bool FileWatcher::WatchFile(h64 hash) {
		u64 time = CheckTime(hash);
		if (time == 0)
			return false;

		_hashes.Push(hash);
		_times.Push(time);

		return true;
	}

	bool FileWatcher::CheckNext(u32 count, h64& outHash) {
		u32 size = _hashes.Count();
		u32 checkCount = core::Min<u32>(count, size);

		for (u32 i = 0; i < checkCount; ++i, ++_index) {
			_index = _index % size;

			u64 time = CheckTime(_hashes[_index]);
			if (time == 0)
				continue;

			if (time != _times[_index]) {
				_times[_index] = time;

				outHash = _hashes[_index];
				return true;
			}
		}
		return false;
	}

	u64 FileWatcher::CheckTime(h64 hash) const {
		const char* path = _pathBank->Get(hash);
		core::FileHandle handle = core::FileOpenSync(path);

		if (!core::IsFileHandleValid(handle))
			return 0;

		u64 time = core::FileModificationTime(handle);

		core::FileClose(handle);
		return time;
	}
}
