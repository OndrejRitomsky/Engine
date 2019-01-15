#include "texture_manager.h"

#include <core/common/debug.h>
#include <core/algorithm/cstring.h>
#include <core/allocator/allocate.h>
#include <core/collection/lookuparray.inl>
#include <core/collection/hashmap.inl>

#include "../resource_event.h"
#include "../resource_util.h"
#include "../resources.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPG
#include <Extern/Include/stb/stb_image.h>

namespace eng {

	// OUR ALLOCATION should be provided
	// STBI_MALLOC, STBI_REALLOC, and STBI_FREE
	void Texture_Init(Texture* texture, const void* rawData, u32 size) {
		int chann = 0;

		int* width = (int*) &texture->width;
		int* height = (int*) &texture->height;

		texture->data = (void*) stbi_load_from_memory(static_cast<const stbi_uc*>(rawData), size, width, height, &chann, 0);

		switch (chann) {
		case 4: texture->format = TextureFormat::RGBA; break;
		case 3: texture->format = TextureFormat::RGB; break;
		default: ASSERT(false); break;
		}
	}

	void Texture_Deinit(Texture* texture) {
		stbi_image_free(texture->data);
	}

	ResourceTypeFactoryParseFunction(TextureManager::TryCreateTexture) {
		const char* buffer = (const char*) resourceDescription;

		TextureManager* manager = (TextureManager*) factory;

		char* data = (char*) Allocate(manager->_allocator, resourceDescriptionSize, sizeof(char));

		core::Memcpy(data, buffer, resourceDescriptionSize);
		Texture texture;
		Texture_Init(&texture, data, resourceDescriptionSize);

		package->dependenciesCount = 0;
		package->dependant.resource = manager->AddTexture(texture);
	}

	ResourceTypeFactoryCreateFunction(TextureManager::CreateTexture) {
		ASSERT(false);
	}

	TextureManager::TextureManager()  {
		static_assert(offsetof(TextureManager, _factoryInterface) == 0, "Factory interface has to be first");
	}

	TextureManager::~TextureManager() {
	}

	void TextureManager::Init(core::IAllocator* allocator) {
		_textures.Init(allocator, 128);
		_allocator = allocator;
		_factoryInterface.TryCreate = TryCreateTexture;
		_factoryInterface.Create = CreateTexture;
	}

	IResourceTypeFactory* TextureManager::Factory() {
		return &_factoryInterface;
	}

	void TextureManager::RemoveTexture(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		Texture_Deinit(&_textures.Get(handle));
		_textures.Remove(handle);
	}

	const Texture& TextureManager::GetTexture(const Resource& resource) const {
		return _textures.Get(ResourceToHandle(resource));
	}

	Resource TextureManager::AddTexture(const Texture& stage) {
		return HandleToResource(_textures.Add(stage));
	}

}

