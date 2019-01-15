#pragma once

#include <core/common/types.h>
#include <core/collection/array.h>

#include "command/command_parameters.h"
#include "command/command_proxy.h"

namespace core {
	struct IAllocator;
}

namespace render {
	struct JobPackage;
	struct CommandProxy;
	struct Resource;

	class RenderContextStream;

	// not liking the array for commands

	class RenderContext {
	public:
		RenderContext();
		~RenderContext() = default;

		void Init(RenderContextStream* contextStream, core::IAllocator* allocator);

		void BeginStateCommands(SortKey sortKey);
		void AddClearCommand(ClearFlags clearFlags);
		void AddViewPortCommand(i32 width, i32 height);
		void EndStateCommands();

		// JobPackage .. with prototype return pointer to copy for patching ( EX. uniforms)
		JobPackage* AddRenderJobPackage(const JobPackage* jobPackage);

		const CommandProxy* GetCommands(u32& outCount) const;

	private:
		char* AddCommandAndData(const void* data, u64 size, SortKey sortKey);

		// @TODO alignment
		char* AddCommandAndReserve(u64 size, SortKey sortKey);

	private:
		bool _isRecordingStateCommand;

		u64 _stateCommandSortKey;

		core::Array<CommandProxy> _commands;
		RenderContextStream* _contextStream;
	};
}