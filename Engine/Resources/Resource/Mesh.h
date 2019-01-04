#pragma once

#include <core/common/types.h>

#include "resource.h"

namespace eng {



	struct MeshVertexBuffer {
		u32 bufferSize;
		f32* buffer;
	};

	static const u32 MESH_INDEX_BYTE_SIZE = 2; // u16

	struct MeshIndexBuffer {
		u32 bufferSize;
		void* buffer;
	};

	struct MeshDescription {
		MeshVertexBuffer vertexBuffer;
		MeshIndexBuffer indexBuffer;
		h64 hash;
	};

	struct Mesh {
		MeshVertexBuffer vertexBuffer;
		MeshIndexBuffer indexBuffer;
		//Resource handle;
	};
}


