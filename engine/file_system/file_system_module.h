#pragma once

#include <core/common/types.h>

#include "../module.h"

#include "file_system_event.h"

namespace eng {
	struct Engine;
	struct Frame;
	struct PermanentAllocator;

	class IOManager;
	class StringHashBank;

	class FileSystemModule {
	public:
		FileSystemModule();
		~FileSystemModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		void Update(const Frame* frame);

		void OnEventsByType(const Frame* frame, const void* events, FileSystemEventType eventsType);

		void QueryEventsByType(FileSystemEventType type, void* outEvents);

		void ClearPendingEvents();

	private:
		ModuleState _state;

		StringHashBank* _pathBank;
		IOManager* _ioManager;
	};
}