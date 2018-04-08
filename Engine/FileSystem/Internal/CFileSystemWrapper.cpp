#include "CFileSystemWrapper.h"

#include <Core/Algorithm/Move.h>
#include <Core/Algorithm/Memory.h>
#include <Core/Collection/LookupArray.inl>
#include <Core/Collection/HandleMap.inl>

#include <Platform/Win32/FileSystemAPI.h>

namespace eng {

	const u32 MAXIMUM_OPEN_HANDLES = 8;


	struct CFileSystemWrapper::HandleInfo {
		HandleStatus status;
		u32 ioHandle;
		win::FileHandle fileHandle;
		u64 size;		
		const char* path;
	};

	struct CFileSystemWrapper::IOInfo {
		u32 overlappedIndex; // is overlapped index
		core::Handle handle;
		u64 bufferSize;
		char* buffer;
	};

	//---------------------------------------------------------------------------
	win::CompletionPortHandle ToCompletitionQueueHandle(void* queue) {
		return static_cast<win::CompletionPortHandle>(queue);
	}

	//---------------------------------------------------------------------------
	void* FromCompletitionQueueHandle(win::CompletionPortHandle queue) {
		return static_cast<void*>(queue);
	}

	//---------------------------------------------------------------------------
	void OpenSizeClose(CFileSystemWrapper::HandleInfo* info) {
		win::FileHandle fileHandle = win::FileOpenSync(info->path);
		if (win::IsFileHandleValid(fileHandle)) {
			info->size = win::FileSize(fileHandle);
			info->status = info->size == 0 ? HandleStatus::Error : HandleStatus::SizeKnown;
			win::FileClose(fileHandle);
		}
	}

	//---------------------------------------------------------------------------
	CFileSystemWrapper::IOInfo CreateIOInfo(core::Handle handle, char* buffer, u64 size) {
		CFileSystemWrapper::IOInfo io;
		io.buffer = buffer;
		io.bufferSize = size;
		io.handle = handle;
		return io;
	}



	//---------------------------------------------------------------------------
	CFileSystemWrapper::CFileSystemWrapper() :
		_allocator(nullptr),
		_overlappedStorage(nullptr),
		_overlapped(nullptr),
		_openHandlesCount(0),
		_overlappedCount(0),
		_completionQueue(FromCompletitionQueueHandle(win::MakeInvalidFileHandle())) {
	}

	//---------------------------------------------------------------------------
	CFileSystemWrapper::~CFileSystemWrapper() {
		win::CompletionPortClose(ToCompletitionQueueHandle(_completionQueue));

		ASSERT(!_handleInfos.Count());
		ASSERT(!_openHandlesCount);
		ASSERT(!_overlappedCount);

		for (auto& ioData : _handleInfos) {
			_ioInfos.Remove(ioData.ioHandle);
		}
		

		_allocator->Deallocate(_overlapped);
		_allocator->Deallocate(_overlappedStorage);
	}

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::Init(core::IAllocator* allocator) {
		_allocator = allocator;
		_handleInfos.Init(allocator);

		u32 overlappedCount = MAXIMUM_OPEN_HANDLES + 10;

		_ioInfos.Init(allocator, overlappedCount);
		_overlappedStorage = _allocator->Allocate(sizeof(win::Overlapped) * overlappedCount, alignof(win::Overlapped));
		_overlapped = static_cast<win::Overlapped**>(_allocator->Allocate(sizeof(win::Overlapped*) * overlappedCount, alignof(win::Overlapped*)));
		
		for (u32 i = 0; i < overlappedCount; ++i)
			_overlapped[i] = static_cast<win::Overlapped*>(_overlappedStorage) + i;

		_completionQueue = FromCompletitionQueueHandle(win::CompletionPortCreate(0));
	}

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::Update() {
		while (true) {
			win::CompletionKey key;
			win::Overlapped* overlapped;
			win::CompletionPortStatus status = win::CompletionPortQuery(ToCompletitionQueueHandle(_completionQueue), key, overlapped);
			if (status == win::CompletionPortStatus::Empty)
				break;

			IOInfo& io = _ioInfos.Get(key);
			HandleInfo& handleInfo = _handleInfos.Get(io.handle);

			if (status == win::CompletionPortStatus::Error) {
				handleInfo.status = HandleStatus::Error;
			}
			else if (status == win::CompletionPortStatus::Ok) {
				handleInfo.status = HandleStatus::Finished;
				win::FileClose(handleInfo.fileHandle);
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
				info.fileHandle = win::FileOpenAsync(info.path);
				if (win::IsFileHandleValid(info.fileHandle)) {
					info.size = win::FileSize(info.fileHandle);
					if (!info.size) {
						win::FileClose(info.fileHandle);
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

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::CreateIOHandles(u32 count, core::Handle* outHandles) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo info;
			Memset(&info, 0, sizeof(info));
			info.fileHandle = win::MakeInvalidFileHandle();
			info.status = HandleStatus::Created;
			outHandles[i] = _handleInfos.Add(core::move(info));
		}
	}

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::OpenFiles(const core::Handle* handles, const char* const* paths, u32 count) {
		u32 i = 0;
		while (_openHandlesCount < MAXIMUM_OPEN_HANDLES && i < count) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			info.path = paths[i];
			ASSERT(info.status == HandleStatus::Created);
			info.status = HandleStatus::Error;

			win::FileHandle fileHandle = win::FileOpenAsync(paths[i]);
			if (win::IsFileHandleValid(fileHandle)) {
				++_openHandlesCount;
				info.fileHandle = fileHandle;
				info.status = HandleStatus::Open;
			}
			++i;
		}

		for (; i < count; ++i)
			_handleInfos.Get(handles[i]).path = paths[i];
	}

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::GetSize(const core::Handle* handles, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& info = _handleInfos.Get(handles[i]);

			if (info.status == HandleStatus::Open) {
				info.size = win::FileSize(info.fileHandle);
				info.status = info.size == 0 ? HandleStatus::Error : HandleStatus::OpenSizeKnown;
			}
			else if (info.status == HandleStatus::Created) {
				OpenSizeClose(&info);
			}
		}
	}

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::OpenFilesGetSize(const core::Handle* handles, const char* const* paths, u32 count) {
		u32 i = 0;
		while (_openHandlesCount < MAXIMUM_OPEN_HANDLES && i < count) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			info.path = paths[i];
			ASSERT(info.status == HandleStatus::Created);

			info.status = HandleStatus::Error;
			info.fileHandle = win::FileOpenAsync(paths[i]);
			if (win::IsFileHandleValid(info.fileHandle)) {
				info.size = win::FileSize(info.fileHandle);
				if (info.size == 0) {
					win::FileClose(info.fileHandle);
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

	void CFileSystemWrapper::AsyncReadFile(HandleInfo* handleInfo, IOInfo& io) {
		handleInfo->status = HandleStatus::Error;

		if (io.bufferSize < handleInfo->size) {
			ASSERT(false);
			return;
		}

		win::CompletionIOParams params;
		io.overlappedIndex = _overlappedCount;
		params.id = static_cast<u32>(_ioInfos.Add(io));
		params.completionPort = ToCompletitionQueueHandle(_completionQueue);
		params.file = handleInfo->fileHandle;
		params.overlapped = _overlapped[io.overlappedIndex];

		win::Error err = win::FileReadAsync(params, io.buffer, static_cast<u32>(io.bufferSize));

		if (err != win::Error::Ok)
			_ioInfos.Remove(params.id);

		if (err == win::Error::Ok) {
			++_overlappedCount;
			handleInfo->status = HandleStatus::ReadPending;
			handleInfo->ioHandle = params.id;
		}
		else if (err != win::Error::Unknown) { // unknown will just return status error
			ASSERT(false);
		}
	}

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::AsyncReadFiles(const core::Handle* handles, char* const* buffers, u64* buffersSizes, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& handleInfo = _handleInfos.Get(handles[i]);

			if (handleInfo.status == HandleStatus::OpenSizeKnown) {
				ASSERT(_overlappedCount <= _openHandlesCount);
				AsyncReadFile(&handleInfo, CreateIOInfo(handles[i], buffers[i], buffersSizes[i]));
			}
			else if (handleInfo.status == HandleStatus::SizeKnown) {
				if (_openHandlesCount < MAXIMUM_OPEN_HANDLES) {
					handleInfo.fileHandle = win::FileOpenSync(handleInfo.path);

					if (!win::IsFileHandleValid(handleInfo.fileHandle)) {
						handleInfo.status = HandleStatus::Error;
					}
					else {
						ASSERT(_overlappedCount <= _openHandlesCount);
						AsyncReadFile(&handleInfo, CreateIOInfo(handles[i], buffers[i], buffersSizes[i]));
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

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::CloseFiles(const core::Handle* handles, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			switch (info.status) {
			case HandleStatus::ReadPending:
				info.status = HandleStatus::ReadPendingClosed;
				break;
			case HandleStatus::Open:
			case HandleStatus::OpenSizeKnown:
			case HandleStatus::Finished:
				win::FileClose(info.fileHandle);
				info.fileHandle = win::MakeInvalidFileHandle();
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

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::ReturnHandles(const core::Handle* handles, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			HandleInfo& info = _handleInfos.Get(handles[i]);
			switch (info.status) {
			case HandleStatus::ReadPending:
				info.status = HandleStatus::ReadPendingNoHandle;
				break; 
			case HandleStatus::Open:
			case HandleStatus::OpenSizeKnown:
				ASSERT(false); // should have been closed
				win::FileClose(info.fileHandle);
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

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::QueryStatus(const core::Handle* handles, u32 count, HandleStatus* outFilesStatuses) {
		for (u32 i = 0; i < count; ++i)
			outFilesStatuses[i] = _handleInfos.Get(handles[i]).status;
	}

	//---------------------------------------------------------------------------
	void CFileSystemWrapper::QuerySizes(const core::Handle* handles, u32 count, u64* outFilesSizes) {
		for (u32 i = 0; i < count; ++i)
			outFilesSizes[i] = _handleInfos.Get(handles[i]).size;
	}
}