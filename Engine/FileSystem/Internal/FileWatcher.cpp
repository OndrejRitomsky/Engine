#include "FileWatcher.h"

#include <Core/Collection/Array.inl>

#include <Core/Allocator/IAllocator.h>

#include <Platform/Win32/FileSystemAPI.h>

#include <Core/Common/Utility.h>

#include "Engine/FileSystem/Manager/CStringHashBank.h"

namespace eng {

	//---------------------------------------------------------------------------
	FileWatcher::FileWatcher(core::IAllocator* allocator, CStringHashBank* pathBank) :
		_pathBank(pathBank) {

		_hashes.Init(allocator);
		_times.Init(allocator);
	}

	//---------------------------------------------------------------------------
	bool FileWatcher::WatchFile(h64 hash) {
		u64 time = CheckTime(hash);
		if (time == 0)
			return false;

		_hashes.Push(hash);
		_times.Push(time);

		return true;
	}

	//---------------------------------------------------------------------------
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

	//---------------------------------------------------------------------------
	u64 FileWatcher::CheckTime(h64 hash) const {
		const char* path = _pathBank->Get(hash);
		win::FileHandle handle = win::FileOpenSync(path);

		if (!win::IsFileHandleValid(handle))
			return 0;

		u64 time = win::FileModificationTime(handle);

		win::FileClose(handle);
		return time;
	}
}
