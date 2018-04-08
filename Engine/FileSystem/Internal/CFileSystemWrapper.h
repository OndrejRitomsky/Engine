#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/HandleMap.h>
#include <Core/Collection/LookupArray.h>

namespace win {
	struct Overlapped;
}

namespace eng {
	// @TODO KILL THIS ABOMINATION
	// @TODO KILL THIS ABOMINATION
	// @TODO KILL THIS ABOMINATION
	// @TODO KILL THIS ABOMINATION -----------------> Split files work with requests (will clean up handle enum) 
	// @TODO KILL THIS ABOMINATION
	// @TODO KILL THIS ABOMINATION
	// @TODO KILL THIS ABOMINATION

	// @TODO This can be simplified and fixed since io was splitted from handle
	// @TOOD make this not one file one object!!! and split reading with open close (maybe, since its different handles for same file)
	enum class HandleStatus : u8 {
		Created,             // Handle was created
		Open,                // File is open
		OpenSizeKnown,       // File is open and size (or more info) is known
		SizeKnown,           // File is closed (force closed, maximum handles open handles reached,
		                     //   doesn not have to be reopened to read)

		ReadPending,         // File is being read
		ReadPendingNoHandle, // File is being read but associated handle was returned
		ReadPendingClosed,   // File is being read but file was closed
		ReadOnHold,          // File is closed (maximum handles reached), will be opened when handles are freed

		Finished,            // File IO operation has finished
		Error,               // Error during opening or IO op (unknown name, platform error)
	};


	// @TODO should rework Handles to actual be opaque FileInfo
	// and think how to change the functions so handle map doesnt have to be called

	// Curently only whole files


	// The FileSystemWrapper is keeping number of concurently opened files to implementation maximum
	// Providing api to do operations on multiple files at once
	// And in the future abstract platform implementation // or maybe not
	class CFileSystemWrapper {
	public:
		struct HandleInfo;
		struct IOInfo;
	public:
		CFileSystemWrapper();
		~CFileSystemWrapper();

		void Init(core::IAllocator* allocator);

		void Update();

		void CreateIOHandles(u32 count, core::Handle* outHandles);

		// Only up to maximum amount of available open handles, 
		// rest will be just created (might not be continious, since errors might occure)
		// @TODO ?? 
		void OpenFiles(const core::Handle* handles, const char* const* paths, u32 count);

		// must be open
		void GetSize(const core::Handle* handles, u32 count);

		// will get size from all files which are succesfuly opened and keep open the maximum amount of available free open handles
		void OpenFilesGetSize(const core::Handle* handles, const char* const* paths, u32 count);

		// buffersSizes must be bigger than size of the file (can be queried with other functions)
		// Closes files after success
		void AsyncReadFiles(const core::Handle* handles, char* const* buffers, u64* buffersSizes, u32 count);

		// File is closed if operations arent pending, if they are file will be closed after they are finished and their results is lost
		void CloseFiles(const core::Handle* handles, u32 count);

		void ReturnHandles(const core::Handle* handles, u32 count);

		void QueryStatus(const core::Handle* handles, u32 count, HandleStatus* outStatuses);

		// 0 is size is not know or error happened
		void QuerySizes(const core::Handle* handles, u32 count, u64* outFilesSizes);

	private:
		// todo ref 
		void AsyncReadFile(HandleInfo* handleInfo, IOInfo& io);

	private:
		u32 _openHandlesCount;
		u32 _overlappedCount;

		void* _completionQueue;

		void* _overlappedStorage;
		win::Overlapped** _overlapped;

		core::HandleMap<HandleInfo> _handleInfos;
		core::LookupArray<IOInfo> _ioInfos; // returned index is used as completion key

		core::IAllocator* _allocator;
	};



}