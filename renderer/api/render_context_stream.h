#pragma once

#include <core/common/types.h>

namespace core {
	struct SingleTagProxyAllocator;
}

namespace render {
	// This class wont deallocate any resources allocated
	// Allocator has to be tagged one, so all allocations are cleared after rendering is done
	// Its just helper class to store command data
	class RenderContextStream {
	public:
		RenderContextStream();
		~RenderContextStream() = default;

		void Init(core::SingleTagProxyAllocator* allocator);

		char* ReserveSize(u64 size);
		
	private:
		bool Reserve(u64 size);

	private:
		u64 _bufferCapacity;
		u64 _bufferSize;
		char* _buffer;

		core::SingleTagProxyAllocator* _allocator;
	};
}

