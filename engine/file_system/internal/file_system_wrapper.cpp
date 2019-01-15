#include "file_system_wrapper.h"

#include <core/algorithm/move.h>
#include <core/algorithm/cstring.h>
#include <core/collection/lookuparray.inl>
#include <core/collection/handlemap.inl>

#include <core/platform/file_system.h>

namespace eng {

	const u32 MAXIMUM_OPEN_HANDLES = 8;


	struct FileSystemWrapper::HandleInfo {
		HandleStatus status;
		u32 ioHandle;
		core::FileHandle fileHandle;
		u64 size;		
		const char* path;
	};

	struct FileSystemWrapper::IOInfo {
		u32 overlappedIndex; // is overlapped index
		core::Handle handle;
		u64 bufferSize;
		char* buffer;
	};

	core::CompletionPortHandle ToCompletitionQueueHandle(void* queue) {
		return static_cast<core::CompletionPortHandle>(queue);
	}

	void* FromCompletitionQueueHandle(core::CompletionPortHandle queue) {
		return static_cast<void*>(queue);
	}

	void OpenSizeClose(FileSystemWrapper::HandleInfo* info) {
		core::FileHandle fileHandle = core::FileOpenSync(info->path);
		if (core::IsFileHandleValid(fileHandle)) {
			info->size = core::FileSize(fileHandle);
			info->status = info->size == 0 ? HandleStatus::Error : HandleStatus::SizeKnown;
			core::FileClose(fileHandle);
		}
	}

	FileSystemWrapper::IOInfo CreateIOInfo(core::Handle handle, char* buffer, u64 size) {
		FileSystemWrapper::IOInfo io;
		io.buffer = buffer;
		io.bufferSize = size;
		io.handle = handle;
		return io;
	}

	FileSystemWrapper::FileSystemWrapper() :
		_allocator(nullptr),
		_overlappedStorage(nullptr),
		_overlapped(nullptr),
		_openHandlesCount(0),
		_overlappedCount(0),
		_completionQueue(FromCompletitionQueueHandle(core::MakeInvalidFileHandle())) {
	}

	FileSystemWrapper::~FileSystemWrapper() {
		core::CompletionPortClose(ToCompletitionQueueHandle(_completionQueue));

		ASSERT(!_handleInfos.Count());
		ASSERT(!_openHandlesCount);
		ASSERT(!_overlappedCount);

		for (auto& ioData : _handleInfos) {
			_ioInfos.Remove(ioData.ioHandle);
		}
		

		Deallocate(_allocator, _overlapped);
		Deallocate(_allocator, _overlappedStorage);
	}

	void FileSystemWrapper::Init(core::IAllocator* allocator) {
		_allocator = allocator;
		_handleInfos.Init(allocator);

		u32 overlappedCount = MAXIMUM_OPEN_HANDLES + 10;

		_ioInfos.Init(allocator, overlappedCount);
		_overlappedStorage = Allocate(_allocator, sizeof(core::Overlapped) * overlappedCount, alignof(core::Overlapped));
		_overlapped = static_cast<core::Overlapped**>(Allocate(_allocator, sizeof(core::Overlapped*) * overlappedCount, alignof(core::Overlapped*)));
		
		for (u32 i = 0; i < overlappedCount; ++i)
			_overlapped[i] = static_cast<core::Overlapped*>(_overlappedStorage) + i;

		_completionQueue = FromCompletitionQueueHandle(core::CompletionPortCreate(0));
	}

	void FileSystemWrapper::Update() {
		while (true) {
			core::CompletionKey key;
			core::Overlapped* overlapped;
			core::CompletionPortStatus status = core::CompletionPortQuery(ToCompletitionQueueHandle(_completionQueue), key, overlapped);
			if (status == core::CompletionPortStatus::Empty)
				break;

			IOInfo& io = _ioInfos.Get(key);
			HandleInfo& handleInfo = _handleInfos.Get(io.handle);

			if (status == core::CompletionPortStatus::Error) {
				handleInfo.status = HandleStatus::Error;
			}
			else if (status == core::CompletionPortStatus::Ok) {
				handleInfo.status = HandleStatus::Finished;
				core::FileClose(handleInfo.fileHandle);
				--_openHandlesCount;
			}

			_overlapped[io.overlappedIndex] = _overlapped[_overlappedCount - 1]; // move last to the index
			--_overlappedCount;
			_ioInfos.Remove(key);
		}

		for (HandleInfo& info : _handleInfos) {
			if (_openHandlesCount >= MAXIMUM_OPEN_HANDLES)
				break;

			if (info.status == HandleStatus::ReadOnHold) {
				info.fileHandle = core::FileOpenAsync(info.path);
				if (core::IsFileHandleValid(info.fileHandle)) {
					info.size = core::FileSize(info.fileHandle);
					if (!info.size) {
						core::FileClose(info.fileHandle);
					}
					else {
						++_openHandlesCount;
					}
				}

				IOInfo& io = _ioInfos.Get(info.ioHandle);
				AsyncReadFile(&info, io);
			}
		}
	}

	void FileSystemWrapper::CreateIOHandles(u32 count, core::Handle* outHandles) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo info;
			core::Memset(&info, 0, sizeof(info));
			info.fileHandle = core::MakeInvalidFileHandle();
			info.status = HandleStatus::Created;
			outHandles[i] = _handleInfos.Add(core::move(info));
		}
	}

	void FileSystemWrapper::OpenFiles(const core::Handle* handles, const char* const* paths, u32 count) {
		u32 i = 0;
		while (_openHandlesCount < MAXIMUM_OPEN_HANDLES && i < count) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			info.path = paths[i];
			ASSERT(info.status == HandleStatus::Created);
			info.status = HandleStatus::Error;

			core::FileHandle fileHandle = core::FileOpenAsync(paths[i]);
			if (core::IsFileHandleValid(fileHandle)) {
				++_openHandlesCount;
				info.fileHandle = fileHandle;
				info.status = HandleStatus::Open;
			}
			++i;
		}

		for (; i < count; ++i)
			_handleInfos.Get(handles[i]).path = paths[i];
	}

	void FileSystemWrapper::GetSize(const core::Handle* handles, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& info = _handleInfos.Get(handles[i]);

			if (info.status == HandleStatus::Open) {
				info.size = core::FileSize(info.fileHandle);
				info.status = info.size == 0 ? HandleStatus::Error : HandleStatus::OpenSizeKnown;
			}
			else if (info.status == HandleStatus::Created) {
				OpenSizeClose(&info);
			}
		}
	}


	void FileSystemWrapper::OpenFilesGetSize(const core::Handle* handles, const char* const* paths, u32 count) {
		u32 i = 0;
		while (_openHandlesCount < MAXIMUM_OPEN_HANDLES && i < count) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			info.path = paths[i];
			ASSERT(info.status == HandleStatus::Created);

			info.status = HandleStatus::Error;
			info.fileHandle = core::FileOpenAsync(paths[i]);
			if (core::IsFileHandleValid(info.fileHandle)) {
				info.size = core::FileSize(info.fileHandle);
				if (info.size == 0) {
					core::FileClose(info.fileHandle);
				}
				else {
					info.status = HandleStatus::OpenSizeKnown;
					++_openHandlesCount;
				}
			}
			++i;
		}

		for (; i < count; ++i) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			info.path = paths[i];
			ASSERT(info.status == HandleStatus::Created);
			OpenSizeClose(&info);
		}
	}

	void FileSystemWrapper::AsyncReadFile(HandleInfo* handleInfo, IOInfo& io) {
		handleInfo->status = HandleStatus::Error;

		if (io.bufferSize < handleInfo->size) {
			ASSERT(false);
			return;
		}

		core::CompletionIOParams params;
		io.overlappedIndex = _overlappedCount;
		params.id = static_cast<u32>(_ioInfos.Add(io));
		params.completionPort = ToCompletitionQueueHandle(_completionQueue);
		params.file = handleInfo->fileHandle;
		params.overlapped = _overlapped[io.overlappedIndex];

		core::Error err = core::FileReadAsync(params, io.buffer, static_cast<u32>(io.bufferSize));

		if (err != core::Error::Ok)
			_ioInfos.Remove(params.id);

		if (err == core::Error::Ok) {
			++_overlappedCount;
			handleInfo->status = HandleStatus::ReadPending;
			handleInfo->ioHandle = params.id;
		}
		else if (err != core::Error::Unknown) { // unknown will just return status error
			ASSERT(false);
		}
	}

	void FileSystemWrapper::AsyncReadFiles(const core::Handle* handles, char* const* buffers, u64* buffersSizes, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& handleInfo = _handleInfos.Get(handles[i]);

			if (handleInfo.status == HandleStatus::OpenSizeKnown) {
				ASSERT(_overlappedCount <= _openHandlesCount);
				auto ioInfo = CreateIOInfo(handles[i], buffers[i], buffersSizes[i]);
				AsyncReadFile(&handleInfo, ioInfo);
			}
			else if (handleInfo.status == HandleStatus::SizeKnown) {
				if (_openHandlesCount < MAXIMUM_OPEN_HANDLES) {
					handleInfo.fileHandle = core::FileOpenSync(handleInfo.path);

					if (!core::IsFileHandleValid(handleInfo.fileHandle)) {
						handleInfo.status = HandleStatus::Error;
					}
					else {
						ASSERT(_overlappedCount <= _openHandlesCount);
						auto ioInfo = CreateIOInfo(handles[i], buffers[i], buffersSizes[i]);
						AsyncReadFile(&handleInfo, ioInfo);
					}
				}
				else {
					handleInfo.status = HandleStatus::ReadOnHold;
					_ioInfos.Add(CreateIOInfo(handles[i], buffers[i], buffersSizes[i]));
				}
			}
			else {
				ASSERT(false);
			}
		}
	}

	void FileSystemWrapper::CloseFiles(const core::Handle* handles, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			switch (info.status) {
			case HandleStatus::ReadPending:
				info.status = HandleStatus::ReadPendingClosed;
				break;
			case HandleStatus::Open:
			case HandleStatus::OpenSizeKnown:
			case HandleStatus::Finished:
				core::FileClose(info.fileHandle);
				info.fileHandle = core::MakeInvalidFileHandle();
			case HandleStatus::ReadOnHold:
				--_openHandlesCount;
				break;
			case HandleStatus::SizeKnown:
			case HandleStatus::Created:
			case HandleStatus::Error:
			case HandleStatus::ReadPendingNoHandle:
			case HandleStatus::ReadPendingClosed:
			default:
				ASSERT(false);
				break;
			}

		}
	}

	void FileSystemWrapper::ReturnHandles(const core::Handle* handles, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			switch (info.status) {
			case HandleStatus::ReadPending:
				info.status = HandleStatus::ReadPendingNoHandle;
				break; 
			case HandleStatus::Open:
			case HandleStatus::OpenSizeKnown:
				ASSERT(false); // should have been closed
				core::FileClose(info.fileHandle);
			case HandleStatus::ReadOnHold:
				--_openHandlesCount;
			case HandleStatus::SizeKnown: 
			case HandleStatus::Created:
			case HandleStatus::Finished:
			case HandleStatus::Error:
			case HandleStatus::ReadPendingClosed:
			{
				//auto& ioData = _handleInfos.Get(handles[i]);
			//	_ioInfos.Remove(ioData.ioHandle);
				_handleInfos.Remove(handles[i]);
				break;
			}
			case HandleStatus::ReadPendingNoHandle:
			default:
				ASSERT(false);
				break;
			}
		}
	}

	void FileSystemWrapper::QueryStatus(const core::Handle* handles, u32 count, HandleStatus* outFilesStatuses) {
		for (u32 i = 0; i < count; ++i)
			outFilesStatuses[i] = _handleInfos.Get(handles[i]).status;
	}

	void FileSystemWrapper::QuerySizes(const core::Handle* handles, u32 count, u64* outFilesSizes) {
		for (u32 i = 0; i < count; ++i)
			outFilesSizes[i] = _handleInfos.Get(handles[i]).size;
	}
}