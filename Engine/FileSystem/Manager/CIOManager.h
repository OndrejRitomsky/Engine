#pragma once

#include <core/collection/hashmap.h>

#include "Engine/FileSystem/FileSystemEvent.h"
#include "Engine/FileSystem/Internal/CFileSystemWrapper.h"

namespace eng {
	class CStringHashBank;

	struct FileSystemEventLoad;

	class CIOManager {
	private:
		struct LoadData;

	public:
		CIOManager();
		~CIOManager();

		void Init(core::IAllocator* allocator, const CStringHashBank* stringBank);

		void Load(const FileSystemEventLoad* loadEvents, u32 eventsCount);

		void Update();

		void QueryEventsByType(FileSystemEventType type, void* outEvents);

		void ClearPendingEvents();

	private:
		// @TODO this should be refactored into separate soa structure
		FileSystemLoadedEvents _loadsFinished;

		const CStringHashBank* _stringBank;

		CFileSystemWrapper _fileSystem;

		core::HashMap<core::Handle> _resourceToHandleMap;
		core::HashMap<LoadData> _resourceToLoadData;

		core::IAllocator* _allocator;
	};
}