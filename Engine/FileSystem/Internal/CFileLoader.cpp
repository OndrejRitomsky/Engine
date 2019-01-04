#include "CFileLoader.h"

#include <core/common/debug.h>
#include <core/allocator/allocate.h>
#include <core/collection/hashmap.inl>
#include <core/collection/handlemap.inl>

#include <stdio.h>

namespace eng {
	using namespace core;

	// EROOORR HOW TO DISTINGUISH if we cleaned up file or no
	// its cleaned if its invalid file handle

	//---------------------------------------------------------------------------
	CFileLoader::CFileLoader() :
		_allocator(nullptr),
		_currentId(0),
		_loadingCount(0),
		_queueHandle(core::MakeInvalidFileHandle()) {
	}

	//---------------------------------------------------------------------------
	CFileLoader::~CFileLoader() {
		// clean rest of the files!!!!!!!!!!!!!!!!!

		core::CompletionPortClose(_queueHandle);
	}

	//---------------------------------------------------------------------------
	void CFileLoader::Init(IAllocator* allocator) {
		_queueHandle = core::CompletionPortCreate(1);
		_allocator = allocator;
		_idToHandleMap.Init(allocator);
		_ids.Init(allocator);
	}

	//---------------------------------------------------------------------------
	void CFileLoader::Update() {

		core::CompletionKey id;

		core::CompletionPortStatus result = core::CompletionPortStatus::Ok;

		while (true) {
			core::Overlapped* overlapped;

			result = core::CompletionPortQuery(_queueHandle, id, overlapped);

			if (result == core::CompletionPortStatus::Empty)
				break;

			Deallocate(_allocator, overlapped);

			Handle* handle = _idToHandleMap.Find(id);

			if (!handle || !_ids.IsValid(*handle)) {
				ASSERT(false); // entry will stuck
				continue;
			}

			Entry& entry = _ids.Get(*handle);

			if (result == core::CompletionPortStatus::Error)
				entry.status = Status::Error;

			if (result == core::CompletionPortStatus::Ok)
				entry.status = Status::Ok;
		}
	}

	//---------------------------------------------------------------------------
	CFileLoader::Status CFileLoader::AsyncLoad(IAllocator* allocator, const char* path, Handle& outHandle) {
		Entry entry;
		entry.id = _currentId++;
		entry.path = path;
		entry.allocator = allocator;
		entry.status = Status::OnHold;

		if (_loadingCount < MAX_HANDLE_COUNT)
			entry = StartLoading(entry);

		if (entry.status == Status::Pending || entry.status == Status::OnHold) {
			outHandle = _ids.Add(entry);
			printf("asyncload adding entry %d,  %s \n", entry.id, path);
			_idToHandleMap.Add(entry.id, outHandle);
		}
		else {
			printf("asyncload entry %d not added %s \n", entry.id, path);
		}

		return entry.status;
	}

	//---------------------------------------------------------------------------
	/*void CFileLoader::GetFilesSizes(const char* const* paths, u32 pathsCount, u32* sizesOut) {
		for (u32 i = 0; i < pathsCount; ++i) {
			win::FileHandle fileHandle = win::OpenFile(paths[i], win::AccessType::Sync);

			u32 size = 0;
			if (win::IsFileValid(fileHandle)) {
				size = win::FileSize(fileHandle);
				win::CloseFile(fileHandle);
			}
			sizesOut[i] = size;
		}
	}*/


	//---------------------------------------------------------------------------
	/*void CFileLoader::GetFilesSizesKeepOpen(const char* const* paths, u32 pathsCount, u32* outSizes, core::Handle* outHandles, FileSizeQueryState* outStates) {
		for (u32 i = 0; i < pathsCount; ++i) {
			u32 size = 0;
			FileSizeQueryState state = FileSizeQueryState::Error;

			if (paths[i] == nullptr) {
				outStates[i] = state;
				continue;
			}

			win::FileHandle fileHandle = win::OpenFile(paths[i], win::AccessType::Async);
			if (win::IsFileValid(fileHandle)) {
				size = win::FileSize(fileHandle);

				if (size == 0) {
					win::CloseFile(fileHandle);
				}
				else if (_loadingCount++ >= MAX_HANDLE_COUNT) {
					win::CloseFile(fileHandle);
					state = FileSizeQueryState::OnlySize;
				}
				else {   // if its not max keep open
					Entry entry;
					entry.id = _currentId++;
					entry.path = paths[i];
					entry.allocator = nullptr;
					entry.status = Status::ReadSize;

					Handle h = _ids.Add(entry);
					outHandles[i] = h;
					sizesOut[i] = size;
					_idToHandleMap.Add(entry.id, h);
					state = FileSizeQueryState::Open;
				}
			}

			outStates[i] = state;
		}
	}*/

	//---------------------------------------------------------------------------
	CFileLoader::Status CFileLoader::CheckStatus(Handle handle) {
		if (!_ids.IsValid(handle)) {
			ASSERT(false);
			return Status::Error;
		}

		Entry& entry = _ids.Get(handle);

		if (entry.status == Status::OnHold) {
			if (_loadingCount == MAX_HANDLE_COUNT)
				return Status::Pending;

			entry = StartLoading(entry);

			if (entry.status == Status::Error) {
				_ids.Remove(handle);
				printf("asyncload removing %d ERROR \n", entry.id);
				_idToHandleMap.SwapRemove(entry.id);
			}
		}

		return entry.status;
	}

	//---------------------------------------------------------------------------
	char* CFileLoader::QueryResult(Handle handle) {
		if (!_ids.IsValid(handle)) {
			ASSERT(false);
			return nullptr;
		}

		Entry& entry = _ids.Get(handle);

		if (entry.status != Status::Ok) {
			ASSERT(false);
			return nullptr;
		}

		return entry.data;
	}

	//---------------------------------------------------------------------------
	void CFileLoader::ReturnHandle(Handle handle) {
		if (!_ids.IsValid(handle)) {
			ASSERT(false);
			return;
		}

		Entry& entry = _ids.Get(handle);



		if (core::IsFileHandleValid(entry.fileHandle)) {
			core::FileClose(entry.fileHandle);

			printf("asyncload returning handle %d \n", entry.id);
			_idToHandleMap.SwapRemove(entry.id);
			_ids.Remove(handle);

			ASSERT(_loadingCount > 0);
			_loadingCount--;
		}
	}

	//---------------------------------------------------------------------------
	CFileLoader::Entry CFileLoader::StartLoading(const Entry& entry) {
		Entry newEntry = entry;
		newEntry.status = Status::Error;
		newEntry.fileHandle = core::MakeInvalidFileHandle();

		core::FileHandle fileHandle = core::FileOpenAsync(newEntry.path);

		if (!core::IsFileHandleValid(fileHandle))
			return newEntry;

		u32 size = core::FileSize(fileHandle);

		if (size == 0) {
			core::FileClose(fileHandle);
			return newEntry;
		}

		core::CompletionIOParams params;
		params.id = newEntry.id;
		params.file = fileHandle;
		params.completionPort = _queueHandle;
		params.overlapped = (core::Overlapped*) Allocate(_allocator, sizeof(core::Overlapped), alignof(core::Overlapped)); // internal stuff with our allocator

		// asymetric allocation BLEH
		char* data = (char*) Allocate(newEntry.allocator, size + 1, alignof(char)); // external stuff with input allocator
		data[size] = 0;

		core::Error error = core::FileReadAsync(params, data, size);

		if (error != core::Error::Ok) {
			core::FileClose(fileHandle);
			return newEntry;
		}

		_loadingCount++;

		newEntry.fileHandle = fileHandle;
		newEntry.data = data;
		newEntry.status = Status::Pending;

		return newEntry;
	}



}