#pragma once

#include <Core/Common/Types.h>

#include "Engine/Modules/ModuleState.h"

#include "Engine/FileSystem/FileSystemEvent.h"

namespace eng {
	struct Engine;
	struct Frame;

	class CStaticConstructor;
	class CIOManager;
	class CStringHashBank;

	class CFileSystemModule {
	public:
		CFileSystemModule();
		~CFileSystemModule();

		ModuleState State();
		void ConstructSubsytems(CStaticConstructor* constructor);
		void Init(Engine* engine);

		void Update(const Frame* frame);

		void OnEventsByType(const void* events, FileSystemEventType eventsType);

		void QueryEventsByType(FileSystemEventType type, void* outEvents);

		void ClearPendingEvents();

	private:
		ModuleState _state;

		CStringHashBank* _pathBank;
		CIOManager* _ioManager;
	};
}