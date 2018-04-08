#include "FileSystemAPI.h"

#include <Windows.h>
#include <assert.h>

#include <stdio.h>

namespace win {

	static_assert(sizeof(HANDLE) == sizeof(FileHandle), "Size missmatch");

	static_assert(sizeof(Overlapped) == sizeof(OVERLAPPED), "Size missmatch");
	static_assert(sizeof(Overlapped::_internal) == sizeof(OVERLAPPED::Internal), "Size missmatch");
	static_assert(sizeof(Overlapped::_internalHigh) == sizeof(OVERLAPPED::InternalHigh), "Size missmatch");
	static_assert(sizeof(Overlapped::_offset) == sizeof(OVERLAPPED::Offset), "Size missmatch");
	static_assert(sizeof(Overlapped::_offsetHigh) == sizeof(OVERLAPPED::OffsetHigh), "Size missmatch");
	static_assert(sizeof(Overlapped::_hEvent) == sizeof(OVERLAPPED::hEvent), "Size missmatch");

	//---------------------------------------------------------------------------
	bool IsFileHandleValid(FileHandle handle) {
		return static_cast<HANDLE>(handle) != INVALID_HANDLE_VALUE;
	}

	//---------------------------------------------------------------------------
	FileHandle MakeInvalidFileHandle() {
		return  static_cast<HANDLE>(INVALID_HANDLE_VALUE);
	}

	//---------------------------------------------------------------------------
	FileHandle FileOpenSync(const char* path) {
		return static_cast<FileHandle>(CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
	}

	//---------------------------------------------------------------------------
	FileHandle FileOpenAsync(const char* path) {
		return static_cast<FileHandle>(CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL));
	}

	//---------------------------------------------------------------------------
	u32 FileSize(FileHandle handle) {
		u32 fileSize = GetFileSize(static_cast<HANDLE>(handle), NULL);
		if (fileSize == INVALID_FILE_SIZE)
			fileSize = 0;

		return fileSize;
	}

	//---------------------------------------------------------------------------
	u64 FileModificationTime(FileHandle handle) {
		FILETIME ft;
		BOOL res = ::GetFileTime(handle, NULL, NULL, &ft);
		return res ? (((u64) ft.dwHighDateTime) << 32) + ft.dwLowDateTime : 0;
	}

	//---------------------------------------------------------------------------
	bool FileClose(FileHandle file) {
		return ::CloseHandle(static_cast<HANDLE>(file));
	}

	//---------------------------------------------------------------------------
	Error FileRead(FileHandle fileHandle, char* data, unsigned long size) {
		HANDLE handle = static_cast<HANDLE>(fileHandle);
		DWORD bytesToRead = size;
		DWORD bytesRead = 0;

		bool ok = false;
		while (true) {
			assert(bytesToRead <= size);
			if (!::ReadFile(handle, data + (size - bytesToRead), bytesToRead, &bytesRead, NULL))
				break;

			bytesToRead -= bytesRead;
			if (bytesToRead == 0 || bytesRead == 0)
				return Error::Ok;
		}

		return Error::ReadFail;
	}

	//---------------------------------------------------------------------------
	CompletionPortStatus CompletionPortQuery(CompletionPortHandle cpHandle, CompletionKey& outId, Overlapped*& overlapped) {
		OVERLAPPED* winOverlapped;
		DWORD bytesRead;

		ULONG_PTR id;

		if (GetQueuedCompletionStatus(cpHandle, &bytesRead, &id, &winOverlapped, 0)) {
			assert(id < (1llu << 32) - 1);

			outId = static_cast<CompletionKey>(id);
			overlapped = reinterpret_cast<Overlapped*>(winOverlapped);
			return CompletionPortStatus::Ok;
		}

		/* a = GetLastError();
		printf("%d \n", a);*/

		if (winOverlapped == NULL)
			return CompletionPortStatus::Empty;

		return CompletionPortStatus::Error;
	}

	//---------------------------------------------------------------------------
	Error FileReadAsync(const CompletionIOParams& params, char* data, unsigned long size) {
		HANDLE handle = static_cast<HANDLE>(params.file);

		HANDLE hp = CreateIoCompletionPort(handle, params.completionPort, static_cast<ULONG_PTR>(params.id), NULL);

		if (hp == INVALID_HANDLE_VALUE)
			return Error::Unknown;

		OVERLAPPED* overlapped = reinterpret_cast<OVERLAPPED*>(params.overlapped);
		memset(overlapped, 0, sizeof(OVERLAPPED));

		ReadFile(handle, data, size, NULL, overlapped);
		return Error::Ok;
	}

	//---------------------------------------------------------------------------
	CompletionPortHandle CompletionPortCreate(unsigned int maxThreadCount) {
		return static_cast<CompletionPortHandle>(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, maxThreadCount));
	}

	//---------------------------------------------------------------------------
	void CompletionPortClose(CompletionPortHandle handle) {
		::CloseHandle(static_cast<HANDLE>(handle));
	}
}

