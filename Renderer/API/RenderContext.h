#pragma once

#include <Core/Types.h>

#include "Renderer/API/Command/CommandParameters.h"
#include "Renderer/API/Command/CommandProxy.h"


namespace render {
	struct JobPackage;
	struct CommandProxy;
	struct Resource;

	class RenderContextStream;

	class RenderContext {
	public:
		static const u32 MAX_COMMANDS = 16;

	public:
		RenderContext();
		~RenderContext() = default;

		void Init(RenderContextStream* contextStream);

		void BeginStateCommands(SortKey sortKey);
		void AddClearCommand(ClearFlags clearFlags);
		void EndStateCommands();

		// JobPackage .. with prototype return pointer to copy for patching ( EX. uniforms)
		JobPackage* AddRenderJobPackage(const JobPackage* jobPackage);

		const CommandProxy* GetCommands(u32& outCount) const;

	private:
		char* AddCommandAndData(const void* data, u64 size, SortKey sortKey);
		char* AddCommandAndReserve(u64 size, SortKey sortKey);

	private:
		bool _isRecordingStateCommand;
		u32 _commandsSize;

		u64 _stateCommandSortKey;

		CommandProxy _commands[MAX_COMMANDS]; // @REWORK this could be dynamic array, but with proper allocator

		RenderContextStream* _contextStream;
	};
}