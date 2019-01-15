#include "file_system_module.h"

#include <core/common/debug.h>

#include "../engine.h"

#include "../memory/permanent_allocator.h"
#include "../memory/memory_module.h"

#include "../frame/frame.h"

#include "manager/io_manager.h"
#include "manager/string_hashbank.h"

namespace eng {
	FileSystemModule::FileSystemModule() :
		_state(ModuleState::CREATED),
		_pathBank(nullptr),
		_ioManager(nullptr) {
	}

	FileSystemModule::~FileSystemModule() {
		_pathBank->~StringHashBank();
		_ioManager->~IOManager();
	}

	ModuleState FileSystemModule::State() {
		return _state;
	}

	void FileSystemModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_pathBank = PermanentNew(permanentAllocator, StringHashBank)();
		_ioManager = PermanentNew(permanentAllocator, IOManager)();
	}

	void FileSystemModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();
		_pathBank->Init(all, all);
		_ioManager->Init(all, _pathBank);
	}

	void FileSystemModule::Update(const Frame* frame) {
		_ioManager->Update();
	}

	void FileSystemModule::QueryEventsByType(FileSystemEventType type, void* outEvents) {
		switch (type) {
		case FileSystemEventType::LOADED:
			_ioManager->QueryEventsByType(type, outEvents);
			break;
		default:
			ASSERT(false);
			break;
		}
	}

	void FileSystemModule::OnEventsByType(const Frame* frame, const void* events, FileSystemEventType eventsType) {
		// @TODO SOA
		switch (eventsType) {
		case FileSystemEventType::REGISTER_PATH:
		{
			const FileSystemRegisterPathEvents* rpe = static_cast<const FileSystemRegisterPathEvents*>(events);
			for (u32 i = 0; i < rpe->count; ++i) {
				_pathBank->Add(rpe->hashes[i], rpe->paths[i]);
			}
			break;
		}
		case FileSystemEventType::LOAD:
		{
			const FileSystemLoadEvents* les = static_cast<const FileSystemLoadEvents*>(events);
			for (u32 i = 0; i < les->count; ++i) {
				FileSystemEventLoad le = {les->hashes[i], les->extraData[i]};
				_ioManager->Load(&le, 1); // this shouldnt happen directly, but delayed on update (( What does this mean))
			}
		}
		}
	}

	void FileSystemModule::ClearPendingEvents() {
		_ioManager->ClearPendingEvents();
	}
}