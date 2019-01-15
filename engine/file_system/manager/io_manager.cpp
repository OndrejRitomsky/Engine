#include "io_manager.h"

#include <core/common/debug.h>
#include <core/collection/hashmap.inl>
#include <core/common/pointer.h>

#include <core/algorithm/cstring.h>

#include "string_hashbank.h"

#include "../file_system_event.h"

namespace eng {

	static const u32 LOAD_EVENTS_CAPACITY = 128;


	struct IOManager::LoadData {
		u64 bufferSize;
		char* buffer;
		u64 extraData;
	};

	IOManager::IOManager() :
		_stringBank(nullptr),
		_allocator(nullptr),
		_loadsFinished{0} {
	}

	IOManager::~IOManager() {
		Deallocate(_allocator, _loadsFinished.nameHashes);
	}

	void IOManager::Init(core::IAllocator* allocator, const StringHashBank* stringBank) {
		_stringBank = stringBank;
		_allocator = allocator;

		_fileSystem.Init(allocator);

		_resourceToHandleMap.Init(allocator);
		_resourceToLoadData.Init(allocator);

		const u64 ELEMENTS_SIZE = sizeof(h64) + sizeof(u64) * 2 + sizeof(char*);
		const u64 EXTRA_SIZE = alignof(u64) * 2 + alignof(char*);

		u64 size = LOAD_EVENTS_CAPACITY * ELEMENTS_SIZE + EXTRA_SIZE;

		_loadsFinished.count = 0;
		_loadsFinished.nameHashes = static_cast<h64*>(Allocate(_allocator, size, alignof(h64)));
		_loadsFinished.extraData = static_cast<u64*>(core::PointerAlign(&_loadsFinished.nameHashes[LOAD_EVENTS_CAPACITY], alignof(u64)));
		_loadsFinished.bufferSizes = static_cast<u64*>(core::PointerAlign(&_loadsFinished.extraData[LOAD_EVENTS_CAPACITY], alignof(u64)));
		_loadsFinished.buffers = static_cast<char**>(core::PointerAlign(&_loadsFinished.bufferSizes[LOAD_EVENTS_CAPACITY], alignof(char*)));
	}

	void IOManager::Load(const FileSystemEventLoad* loadEvents, u32 eventsCount) {
		// this shouldnt happen now, it should be stored for later in update
		// @TODO @OPTIMIZE with tag

		ASSERT(eventsCount < 100);

		core::Handle* handles = static_cast<core::Handle*>(Allocate(_allocator, sizeof(core::Handle) * eventsCount, alignof(core::Handle*)));
		const char** paths = static_cast<const char**>(Allocate(_allocator, sizeof(char*) * eventsCount, alignof(char*)));

		for (u32 i = 0; i < eventsCount; ++i) {
			paths[i] = _stringBank->Get(loadEvents[i].nameHash);

			if (!paths[i]) {
				ASSERT(false);
			}
		}


		u64* sizes = static_cast<u64*>(Allocate(_allocator, sizeof(u64) * eventsCount, alignof(u64)));

		_fileSystem.CreateIOHandles(eventsCount, handles);
		_fileSystem.OpenFilesGetSize(handles, paths, eventsCount);
		_fileSystem.QuerySizes(handles, eventsCount, sizes);

		char** buffers = static_cast<char**>(Allocate(_allocator, sizeof(char*) * eventsCount, alignof(char*)));

		for (u32 i = 0; i < eventsCount; ++i) {
			ASSERT(sizes[i]);
			buffers[i] = static_cast<char*>(Allocate(_allocator, sizes[i] + 1, alignof(void*)));
			buffers[i][sizes[i]] = '\0';
		}

		_fileSystem.AsyncReadFiles(handles, buffers, sizes, eventsCount);

		for (u32 i = 0; i < eventsCount; ++i) {
			_resourceToHandleMap.Add(loadEvents[i].nameHash, handles[i]);
			_resourceToLoadData.Add(loadEvents[i].nameHash, LoadData{sizes[i] + 1, buffers[i], loadEvents[i].extraData});
		}

		Deallocate(_allocator, paths);
		Deallocate(_allocator, sizes);
		Deallocate(_allocator, buffers);
		Deallocate(_allocator, handles);
	}

	void IOManager::Update() {
		_fileSystem.Update();

		auto it = _resourceToHandleMap.CIterator(); // This iterating data from which it is deleted too


		u32 count = it.count;

		if (!count)
			return;

		HandleStatus* status = static_cast<HandleStatus*>(Allocate(_allocator, sizeof(HandleStatus) * count, alignof(HandleStatus)));
		_fileSystem.QueryStatus(it.values, count, status);

		u32 i = 0;
		u32 k = 0;
		while (k < count && _loadsFinished.count < LOAD_EVENTS_CAPACITY) {
			core::Handle resourceHash = it.keys[i];
			core::Handle fsHandle = it.values[i];

			if (status[k] == HandleStatus::Finished) {
				LoadData* loadData = _resourceToLoadData.Find(resourceHash);
				ASSERT(loadData);

				loadData->buffer[loadData->bufferSize - 1] = '\0';

				_loadsFinished.nameHashes[_loadsFinished.count] = resourceHash;
				_loadsFinished.extraData[_loadsFinished.count] = loadData->extraData;
				_loadsFinished.bufferSizes[_loadsFinished.count] = loadData->bufferSize;
				_loadsFinished.buffers[_loadsFinished.count] = loadData->buffer;
				++_loadsFinished.count;
			}

			if (status[k] == HandleStatus::Finished || status[k] == HandleStatus::Error) {
				_resourceToHandleMap.SwapRemove(resourceHash);
				_resourceToLoadData.SwapRemove(resourceHash);
				_fileSystem.ReturnHandles(&fsHandle, 1);
			}
			else {
				++i;
			}
			++k;
		}

		ASSERT(_loadsFinished.count < LOAD_EVENTS_CAPACITY); // its ok, just maybe rework it
		Deallocate(_allocator, status);
	}

	void IOManager::QueryEventsByType(FileSystemEventType type, void* outEvents) {
		switch (type) {
		case FileSystemEventType::LOADED:
		{
			FileSystemLoadedEvents* le = static_cast<FileSystemLoadedEvents*>(outEvents);
			core::Memcpy(le, &_loadsFinished, sizeof(FileSystemLoadedEvents));
			break;
		}
		default:
			ASSERT(false);
			break;
		}
	}

	void IOManager::ClearPendingEvents() {
		for (u32 i = 0; i < _loadsFinished.count; ++i) {
			Deallocate(_allocator, _loadsFinished.buffers[i]);
		}

		_loadsFinished.count = 0;
	}
}