#pragma once

#include <core/common/types.h>

#include "resource.h"

namespace eng {

	static const u32 MESH_INDEX_BYTE_SIZE = 2; // u16

	static const u32 MATERIAL_TEXTURES_CAPACITY = 8;

	// --------------------------------------------------------------------------

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
	};

	// --------------------------------------------------------------------------

	struct ShaderStage {
		char* data;
	};

	struct ShaderProgram {
		Resource vertexStage;
		Resource fragmentStage;
	};

	// --------------------------------------------------------------------------

	enum class MeshBufferType : u8 {
		INVALID,
		V2 // float float
	};

	// @TODO type is not yet parsed from files ...

	struct Mesh {
		MeshBufferType positionsType;
		MeshBufferType normalsType;
		MeshBufferType uvsType;
		MeshBufferType colorType;

		u32 indiciesCount;
		u32 verticesCount;

		char* data;

		void* positions; // nullptr if doesnt define
		void* normals;   // nullptr if doesnt define
		void* uvs;       // nullptr if doesnt define
		void* colors;    // nullptr if doesnt define

		void* indices;
	};

	// --------------------------------------------------------------------------

	enum class MaterialUniformType {
		F3,
		F4,
		M44
	};

	struct Material {
		u32 texturesCount;
		Resource textures[MATERIAL_TEXTURES_CAPACITY];
		Resource shaderProgram;
		Resource materialTemplate;
	};

	// --------------------------------------------------------------------------

	struct RenderObject {
		Resource mesh;
		Resource material;
	};
}
