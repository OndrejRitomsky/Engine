#include "texture_manager.h"

#include <core/common/debug.h>
#include <core/collection/lookuparray.inl>

#include "../resource_event.h"
#include "../resource/texture.h"
#include "../resource/resource_util.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <Extern/Include/stb/stb_image.h>

namespace eng {

	// OUR ALLOCATION should be provided
	// STBI_MALLOC, STBI_REALLOC, and STBI_FREE
	//---------------------------------------------------------------------------
	/*void InternalMakeTexture(Texture* texture, const void* rawData, u32 size) {
		int chann = 0;
		 
		int* width = reinterpret_cast<int*>(&texture->width);
		int* height = reinterpret_cast<int*>(&texture->height);

		texture->data = (void*) stbi_load_from_memory(static_cast<const stbi_uc*>(rawData), size, width, height, &chann, 0);

		switch (chann) {
		case 4: texture->format = TextureFormat::RGBA; break;
		case 3: texture->format = TextureFormat::RGB; break;
		default: ASSERT(false); break;
		}
	}

	//---------------------------------------------------------------------------
	void InternalDestroyTexture(Texture* texture) {
		stbi_image_free(texture->data);
	}

	//---------------------------------------------------------------------------
	TextureManager::TextureManager()  {
	}

	//---------------------------------------------------------------------------
	TextureManager::~TextureManager() {
	}

	//---------------------------------------------------------------------------
	void TextureManager::Init(core::IAllocator* allocator) {
		_textures.Init(allocator, 128);
		_allocator = allocator;

		_DependenciesCount = static_cast<IResourceConstructor::DependenciesCountFunction>(&TextureManager::DependenciesCount);
		_FillDependencies = static_cast<IResourceConstructor::FillDependenciesFunction>(&TextureManager::FillDependencies);
		_Create = static_cast<IResourceConstructor::CreateFunction>(&TextureManager::Create);
	}

	//---------------------------------------------------------------------------
	void TextureManager::RemoveTexture(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		InternalDestroyTexture(&_textures.Get(handle));
		_textures.Remove(handle);
	}

	//---------------------------------------------------------------------------
	const Texture& TextureManager::GetTexture(const Resource& resource) const {
		return _textures.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	u32 TextureManager::DependenciesCount(const void* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void TextureManager::FillDependencies(const void* description, ResourceDependencyEvent* outEvents) {

	}

	//---------------------------------------------------------------------------
	void TextureManager::Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		ASSERT(dependencyParams->dependenciesCount == 2);


		//Texture texture;
		//InternalMakeTexture(&texture, data->buffer, data->bufferSize);
		//if (!texture.data) {
		//	ASSERT(false);
		//	return;
		//}
		//outResource = HandleToResource(_textures.Add(texture));
		//
		//_allocator->Deallocate(data->buffer); // @TODO UGLYYYYYYYYYYYYYYYY
		//
		//ShaderProgram program;
		//program.vertexStage = dependencyParams->dependencies[0].resource;
		//program.fragmentStage = dependencyParams->dependencies[1].resource;
		//
		//outHandle = HandleToResource(_shaderPrograms.Add(program));
	}*/
}

