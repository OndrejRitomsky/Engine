#pragma once


#include <core/common/types.h>

#include "../module.h"

namespace core {
	class Window;
}

namespace eng {
	struct Engine;
	struct Frame;
	struct PermanentAllocator;

	class PlatformModule {
	public:
		PlatformModule();
		~PlatformModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine, i32 width, i32 height);

		void Update(const Frame* frame);

		void SwapBackBuffers();

		void* DeviceContext();

		bool WindowSize(i32* outWidth, i32* outHeight);

		bool CloseRequested();

	private:
		void UpdateWindowEvents();

	private:
		ModuleState _state;
		bool _closeRequested;
		bool _windowSizeChanged;
		i32 _windowWidth;
		i32 _windowHeight;
		core::Window* _window;
	};
}

