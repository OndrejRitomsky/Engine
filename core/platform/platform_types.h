#pragma once

#include "../common/types.h"

namespace core {

	enum PlatformCursorType : u8 {
		PLATFORM_CURSOR_INVALID,
		PLATFORM_CURSOR_ARROW,
		PLATFORM_CURSOR_IBEAM,
		PLATFORM_CURSOR_HAND,
		PLATFORM_CURSOR_VERTICAL,
		PLATFORM_CURSOR_HORIZONTAL,
		PLATFORM_CURSOR_TL_BR,
		PLATFORM_CURSOR_TR_BL
	};

	enum PlatformKeyType : u8 { // Just IsDown!
															// 0 - 128 is currently done by WM_CHAR and maps 1 to 1
		PLATFORM_KEY_INVALID = 128,
		PLATFORM_KEY_LEFT,
		PLATFORM_KEY_RIGHT,
		PLATFORM_KEY_UP,
		PLATFORM_KEY_DOWN,
		PLATFORM_KEY_DELETE,
		PLATFORM_KEY_ALT,
		PLATFORM_KEY_CTRL,
		PLATFORM_KEY_SHIFT,
		PLATFORM_KEY_F1,
	};

	enum PlatformEventType : u8 {
		PLATFORM_EVENT_WINDOW_RESIZE,
		PLATFORM_EVENT_CLOSE // no data
	};

	struct PlatformEventWindowResize {
		PlatformEventType event;
		i32 newWidth;
		i32 newHeight;
	};

	struct PlatformMousePos {
		i32 x;
		i32 y;
	};
}