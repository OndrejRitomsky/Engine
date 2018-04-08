#pragma once

#include <Core/Common/Types.h>

namespace eng {

	enum class TextureFormat : u8 {
		RGBA,
		RGB
	};

	struct TextureDescription {
		TextureFormat format;
		void* data;
		h64 hash;
	};

	struct Texture {
		TextureFormat format;
		u32 width;
		u32 height;
		void* data;
	//	Resource handle;
	};
}


