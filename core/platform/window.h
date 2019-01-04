#pragma once

#include "platform_types.h"

#include "../common/types.h"

// @TODO ...

namespace core {
	class Window;
	struct IAllocator;

	Window* WindowInit(IAllocator* allocator, u32 width, u32 height, const char* name);

	void WindowDeinit(Window* window);

	bool WindowUpdate(Window* window);

	void WindowSetCursor(PlatformCursorType cursor);

	void WindowSwapBuffers(Window* window);

	void* WindowHwnd(Window* window);

	void* WindowDeviceContext(Window* window);

	const char* WindowGetEvents(Window* window, u32* outSize);

	const PlatformMousePos* WindowGetMousePosEvents(Window* window, u32* outCount);

	const PlatformKeyType* WindowGetInputEvents(Window* window, u32* outCount);
}