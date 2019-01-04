#pragma once

#include <core/common/types.h>

#include "../module.h"

#include "Engine/FileSystem/FileSystemEvent.h"

namespace eng {
	struct Engine;
	struct Frame;
	struct PermanentAllocator;

	class CIOManager;
	class CStringHashBank;

	class CFileSystemModule {
	public:
		CFileSystemModule();
		~CFileSystemModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		void Update(const Frame* frame);

		void OnEventsByType(const Frame* frame, const void* events, FileSystemEventType eventsType);

		void QueryEventsByType(FileSystemEventType type, void* outEvents);

		void ClearPendingEvents();

	private:
		ModuleState _state;

		CStringHashBank* _pathBank;
		CIOManager* _ioManager;
	};
}