#include "RenderContext.h"

#include <Core/Allocator/IAllocator.h>
#include <Core/Utility.h>

#include "Renderer/API/Command/JobPackage.h"

#include "Renderer/API/Internal/Commands.h"

#include "Renderer/API/RenderContextStream.h"

namespace render {
	// @TODO private methods to static functions

	//-------------------------------------------------------------------------
	RenderContext::RenderContext() :
		_commandsSize(0),
		_contextStream(nullptr),
		_isRecordingStateCommand(false) {

	}

	//-------------------------------------------------------------------------
	void RenderContext::Init(RenderContextStream* contextStream) {
		_contextStream = contextStream;
	}

	//-------------------------------------------------------------------------
	void RenderContext::BeginStateCommands(SortKey sortKey) {
		ASSERT(!_isRecordingStateCommand);

		_stateCommandSortKey = sortKey;
		_isRecordingStateCommand = true;
	}

	//-------------------------------------------------------------------------
	void RenderContext::AddClearCommand(ClearFlags clearFlag) {
		ASSERT(_isRecordingStateCommand);
		ClearCommand command;
		command.commandType = CommandType::CLEAR;
		command.flags = clearFlag;

		AddCommandAndData(&command, sizeof(ClearCommand), _stateCommandSortKey);
	}

	//-------------------------------------------------------------------------
	void RenderContext::EndStateCommands() {
		ASSERT(_isRecordingStateCommand);
		_isRecordingStateCommand = false;
	}

	//-------------------------------------------------------------------------
	JobPackage* RenderContext::AddRenderJobPackage(const JobPackage* jobPackage) {
		ASSERT(!_isRecordingStateCommand);
		ASSERT(jobPackage->size);
		
		// @TODO not liking this size computation
		u64 size = sizeof(RenderJobCommand) - sizeof(JobPackage) + jobPackage->size;

		RenderJobCommand* data = (RenderJobCommand*) AddCommandAndReserve(size, jobPackage->sortKey);
		data->commandType = CommandType::RENDER;
		//memcpy(&data->renderJobPackage, jobPackage, jobPackage->size);
		memcpy(&data->jobPackage, jobPackage, sizeof(JobPackage));

		// job might need fix, but it will probably be outside
		return &data->jobPackage;
	}

	//-------------------------------------------------------------------------
	char* RenderContext::AddCommandAndData(const void* data, u64 size, SortKey sortKey) {
		char* resData = AddCommandAndReserve(size, sortKey);
		memcpy(resData, data, size);
		return resData;
	}

	//-------------------------------------------------------------------------
	char* RenderContext::AddCommandAndReserve(u64 size, SortKey sortKey) {
		ASSERT(_commandsSize < MAX_COMMANDS);
		if (_commandsSize >= MAX_COMMANDS)
			return nullptr;

		char* streamData = _contextStream->ReserveSize(size);
		if (streamData) {
			_commands[_commandsSize].sortKey = sortKey;
			_commands[_commandsSize].data = streamData;
			++_commandsSize;
		}

		return streamData;
	}

	//-------------------------------------------------------------------------
	const CommandProxy* RenderContext::GetCommands(u32& outCount) const {
		outCount = _commandsSize;
		return _commands;
	}
}
