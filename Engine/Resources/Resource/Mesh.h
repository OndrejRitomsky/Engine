#pragma once

#include <Core/Common/Types.h>

#include "Engine/Resources/Resource/Resource.h"

namespace eng {

	/*enum MeshVertexBufferType {

	};*/

	enum class IndexBufferType : u8 {
		U16,
		U32
	};

	struct MeshVertexBuffer {
		u32 bufferSize;
		void* buffer;
	};

	struct MeshIndexBuffer {
		IndexBufferType indexType;
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
		Resource handle;
	};
}


