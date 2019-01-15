#pragma once

#include "../common/types.h"

namespace core {

	// @TODO move types?

	typedef void* FileHandle;
	typedef void* CompletionPortHandle;
	typedef u32 CompletionKey;

	struct Overlapped {
		u64 _internal;
		u64 _internalHigh;
		u32 _offset;
		u32 _offsetHigh;
		void* _hEvent;
	};

	enum class CompletionPortStatus {
		Ok,
		Empty,
		Error
	};

	struct CompletionIOParams {
		CompletionKey id;
		CompletionPortHandle completionPort;
		FileHandle file;
		Overlapped* overlapped;
	};

	// fs error?
	enum class Error {
		Ok,
		ReadFail,
		FileNotFound,
		Unknown
	};

	bool IsFileHandleValid(FileHandle handle);
	FileHandle MakeInvalidFileHandle();

	FileHandle FileOpenSync(const char* path);
	FileHandle FileOpenAsync(const char* path);

	// 0 is invalid file size
	u32 FileSize(FileHandle handle);

	//the number of 100 - nanosecond intervals since January 1, 1601
	u64 FileModificationTime(FileHandle handle);

	bool FileClose(FileHandle handle);

	Error FileRead(FileHandle fileHandle, char* data, unsigned long size);

	Error FileReadAsync(const CompletionIOParams& params, char* data, unsigned long size);

	CompletionPortStatus CompletionPortQuery(CompletionPortHandle cpHandle, CompletionKey& outId, Overlapped*& overlapped);

	CompletionPortHandle CompletionPortCreate(unsigned int maxThreadCount);

	void CompletionPortClose(CompletionPortHandle handle);
}
