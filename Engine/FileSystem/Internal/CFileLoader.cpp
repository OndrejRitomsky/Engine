#include "CFileLoader.h"

#include <Core/Common/Assert.h>
#include <Core/Allocator/IAllocator.h>
#include <Core/Allocator/IAllocator.inl>
#include <Core/Collection/HashMap.inl>
#include <Core/Collection/HandleMap.inl>

namespace eng {
	using namespace core;

	// EROOORR HOW TO DISTINGUISH if we cleaned up file or no
	// its cleaned if its invalid file handle

	//---------------------------------------------------------------------------
	CFileLoader::CFileLoader() :
		_allocator(nullptr),
		_currentId(0),
		_loadingCount(0),
		_queueHandle(win::MakeInvalidFileHandle()) {
	}

	//---------------------------------------------------------------------------
	CFileLoader::~CFileLoader() {
		// clean rest of the files!!!!!!!!!!!!!!!!!

		win::CompletionPortClose(_queueHandle);
	}

	//---------------------------------------------------------------------------
	void CFileLoader::Init(IAllocator* allocator) {
		_queueHandle = win::CompletionPortCreate(1);
		_allocator = allocator;
		_idToHandleMap.Init(allocator);
		_ids.Init(allocator);
	}

	//---------------------------------------------------------------------------
	void CFileLoader::Update() {

		win::CompletionKey id;

		win::CompletionPortStatus result = win::CompletionPortStatus::Ok;

		while (true) {
			win::Overlapped* overlapped;

			result = win::CompletionPortQuery(_queueHandle, id, overlapped);

			if (result == win::CompletionPortStatus::Empty)
				break;

			_allocator->Deallocate(overlapped);

			Handle* handle = _idToHandleMap.Find(id);

			if (!handle || !_ids.IsValid(*handle)) {
				ASSERT(false); // entry will stuck
				continue;
			}

			Entry& entry = _ids.Get(*handle);

			if (result == win::CompletionPortStatus::Error)
				entry.status = Status::Error;

			if (result == win::CompletionPortStatus::Ok)
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



		if (win::IsFileHandleValid(entry.fileHandle)) {
			win::FileClose(entry.fileHandle);

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
		newEntry.fileHandle = win::MakeInvalidFileHandle();

		win::FileHandle fileHandle = win::FileOpenAsync(newEntry.path);

		if (!win::IsFileHandleValid(fileHandle))
			return newEntry;

		u32 size = win::FileSize(fileHandle);

		if (size == 0) {
			win::FileClose(fileHandle);
			return newEntry;
		}

		win::CompletionIOParams params;
		params.id = newEntry.id;
		params.file = fileHandle;
		params.completionPort = _queueHandle;
		params.overlapped = _allocator->Make<win::Overlapped>(); // internal stuff with our allocator

		// asymetric allocation BLEH
		char* data = (char*) newEntry.allocator->Allocate(size + 1, alignof(char)); // external stuff with input allocator
		data[size] = 0;

		win::Error error = win::FileReadAsync(params, data, size);

		if (error != win::Error::Ok) {
			win::FileClose(fileHandle);
			return newEntry;
		}

		_loadingCount++;

		newEntry.fileHandle = fileHandle;
		newEntry.data = data;
		newEntry.status = Status::Pending;

		return newEntry;
	}



}