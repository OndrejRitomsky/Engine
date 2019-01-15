#pragma once

#include <core/collection/hashmap.h>

#include "../file_system_event.h"
#include "../internal/file_system_wrapper.h"

namespace eng {
	class StringHashBank;

	struct FileSystemEventLoad;

	class IOManager {
	private:
		struct LoadData;

	public:
		IOManager();
		~IOManager();

		void Init(core::IAllocator* allocator, const StringHashBank* stringBank);

		void Load(const FileSystemEventLoad* loadEvents, u32 eventsCount);

		void Update();

		void QueryEventsByType(FileSystemEventType type, void* outEvents);

		void ClearPendingEvents();

	private:
		// @TODO this should be refactored into separate soa structure
		FileSystemLoadedEvents _loadsFinished;

		const StringHashBank* _stringBank;

		FileSystemWrapper _fileSystem;

		core::HashMap<core::Handle> _resourceToHandleMap;
		core::HashMap<LoadData> _resourceToLoadData;

		core::IAllocator* _allocator;
	};
}