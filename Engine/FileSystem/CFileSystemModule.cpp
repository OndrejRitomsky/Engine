#include "CFileSystemModule.h"

#include <Core/Common/Assert.h>

#include "Engine/Engine.h"

#include "Engine/Modules/CStaticConstructor.h"

#include "Engine/Memory/CMemoryModule.h"

#include "Engine/FileSystem/Manager/CIOManager.h"
#include "Engine/FileSystem/Manager/CStringHashBank.h"

#include "Engine/Frame/Frame.h"

namespace eng {
	//---------------------------------------------------------------------------
	CFileSystemModule::CFileSystemModule() :
		_state(ModuleState::CREATED),
		_pathBank(nullptr),
		_ioManager(nullptr) {
	}

	//---------------------------------------------------------------------------
	CFileSystemModule::~CFileSystemModule() {
		_pathBank->~CStringHashBank();
		_ioManager->~CIOManager();
	}

	//---------------------------------------------------------------------------
	ModuleState CFileSystemModule::State() {
		return _state;
	}

	//---------------------------------------------------------------------------
	void CFileSystemModule::ConstructSubsytems(CStaticConstructor* constructor) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_pathBank = constructor->Construct<CStringHashBank>();
		_ioManager = constructor->Construct<CIOManager>();
	}

	//---------------------------------------------------------------------------
	void CFileSystemModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();
		_pathBank->Init(all, all);
		_ioManager->Init(all, _pathBank);
	}

	//---------------------------------------------------------------------------
	void CFileSystemModule::Update(const Frame* frame) {
		_ioManager->Update();
	}

	//---------------------------------------------------------------------------
	void CFileSystemModule::QueryEventsByType(FileSystemEventType type, void* outEvents) {
		switch (type) {
		case FileSystemEventType::LOADED:
			_ioManager->QueryEventsByType(type, outEvents);
			break;
		default:
			ASSERT(false);
			break;
		}
	}

	//---------------------------------------------------------------------------
	void CFileSystemModule::OnEventsByType(const void* events, FileSystemEventType eventsType) {
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

	//---------------------------------------------------------------------------
	void CFileSystemModule::ClearPendingEvents() {
		_ioManager->ClearPendingEvents();
	}

}