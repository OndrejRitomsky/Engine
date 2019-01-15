#pragma once

#include <core/collection/lookuparray.h>
#include <core/collection/hashmap.h>

#include "../resource.h"

#include "iresource_type_factory.h"


namespace eng {
	struct Texture;

	class TextureManager {
	public:
		TextureManager();
		~TextureManager();

		TextureManager(const TextureManager& oth) = delete;
		TextureManager& operator=(const TextureManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		// Internal pointer
		IResourceTypeFactory* Factory();

		Resource AddTexture(const Texture& stage);
		const Texture& GetTexture(const Resource& resource) const;
		void RemoveTexture(const Resource& resource);

	private:
		static ResourceTypeFactoryParseFunction(TryCreateTexture);
		static ResourceTypeFactoryCreateFunction(CreateTexture);

	private:
		IResourceTypeFactory _factoryInterface;

		core::IAllocator* _allocator = nullptr;
		core::LookupArray<Texture> _textures;
	};
}