#pragma once

#include <Core/Types.h>

#include "Renderer/API/Resources/Resource.h"

namespace render {
	// Only 2D for now and only generate or not mipmaps

	enum class TextureFilterType : u8 {
		Linear,
		Nearest,
		LinearMipMapLinear
	};

	enum class TextureWrapType : u8 {
		Repeat
	};

	enum class TextureFormat : u8 {
		RGB,
		RGBA
	};

	struct Texture : Resource {
		TextureFilterType minFilter;
		TextureFilterType maxFilter;

		TextureWrapType wrapS;
		TextureWrapType wrapT;

		TextureFormat format;

		bool mipmaps;

		u32 width;
		u32 height;

		void* data;
	};
}