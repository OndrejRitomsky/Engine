#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/HandleMap.h>
#include <Core/Collection/HashMap.h>

#include <Platform/Win32/FileSystemAPI.h> // get rid of this in new loader


// THIS SHOULD BE REFACTORED
// NOW Manages both the loading and waiting for loading

namespace core {
	class IAllocator;
}

namespace eng {


	class CFileLoader {
	public:
		enum class Status : u32 {
			Ok,
			ReadSize,
			Pending,
			OnHold, // internal
			Error,
		};

		enum class FileSizeQueryState : u8 {
			Open,
			OnlySize,
			Error
		};

		struct Entry {
			u32 id;
			Status status;
			win::FileHandle fileHandle;
			char* data;
			const char* path;
			core::IAllocator* allocator;
		};

		const u32 MAX_HANDLE_COUNT = 8;

	public:
		CFileLoader();
		~CFileLoader();

		void Init(core::IAllocator* allocator);

		Status AsyncLoad(core::IAllocator* allocator, const char* path, core::Handle& outHandle);

	/*	void GetFilesSizes(const char* const* paths, u32 pathsCount, u32* sizesOut);
		void GetFilesSizesKeepOpen(const char* const* paths, u32 pathsCount, u32* outSizes, core::Handle* outHandles, FileSizeQueryState* outStates);*/

		void Update();

		Status CheckStatus(core::Handle handle);

		char* QueryResult(core::Handle handle);

		void ReturnHandle(core::Handle handle);

	private:
		Entry StartLoading(const Entry& entry);

	private:
		u32 _loadingCount;
		u32 _currentId;

		win::CompletionPortHandle _queueHandle;

		core::HandleMap<Entry> _ids;

		core::HashMap<core::Handle> _idToHandleMap;

		core::IAllocator* _allocator;
	};

}

