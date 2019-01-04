#pragma once

namespace render {
	struct Resource;
	class RenderContextStream;


	// CAREFUL! resource header are copied but with it its pointer to the resource data is copied too
	// pointer to the data has to be valid until render happens!!! 
	// @TODO ? Maybe change this ^
	// @TODO deadbeef
	// @TODO ? Maybe more than one

	class RenderResourceContext {
	public:
		RenderResourceContext();
		~RenderResourceContext() = default;

		void Allocate(RenderContextStream* contextStream, Resource* resource);
		void Deallocate(RenderContextStream* contextStream, Resource* resource);

		const char* GetData() const;

	private:
		char* _buffer;
	};
}