#include "CTextureManager.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/LookupArray.inl>

#include "Engine/Resources/Resource/Texture.h"
#include "Engine/Resources/Resource/ResourceUtil.h"
#include "Engine/Resources/ResourceEvent.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <Extern/Include/stb/stb_image.h>

namespace eng {

	// OUR ALLOCATION should be provided
	// STBI_MALLOC, STBI_REALLOC, and STBI_FREE
	//---------------------------------------------------------------------------
	void InternalMakeTexture(Texture* texture, const void* rawData, u32 size) {
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
	CTextureManager::CTextureManager()  {
	}

	//---------------------------------------------------------------------------
	CTextureManager::~CTextureManager() {
	}


	//---------------------------------------------------------------------------
	void CTextureManager::Init(core::IAllocator* allocator) {
		_textures.Init(allocator, 128);
		_allocator = allocator;

		_resourceConstructor._DependenciesCount = (IResourceConstructor::DependenciesCountFunction) (&CTextureManager::DependenciesCount);
		_resourceConstructor._FillDependencies = (IResourceConstructor::FillDependenciesFunction) (&CTextureManager::FillDependencies);
		_resourceConstructor._Create = (IResourceConstructor::CreateFunction) (&CTextureManager::Create);
	}

	//---------------------------------------------------------------------------
	IResourceConstructor* CTextureManager::ResourceConstructor() {
		return &_resourceConstructor;
	}

	//---------------------------------------------------------------------------
	void CTextureManager::RemoveTexture(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		InternalDestroyTexture(&_textures.Get(handle));
		_textures.Remove(handle);
	}

	//---------------------------------------------------------------------------
	const Texture& CTextureManager::GetTexture(const Resource& resource) const {
		return _textures.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	u32 CTextureManager::DependenciesCount(const TextureDescription* description) {		
		return 0;
	}

	//---------------------------------------------------------------------------
	void CTextureManager::FillDependencies(const TextureDescription* description, ResourceDependencyEvent* inOutEvents) {

	}

	//---------------------------------------------------------------------------
	void CTextureManager::Create(const TextureDescription* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		ASSERT(dependencyParams->dependenciesCount == 2);


		/*Texture texture;
		InternalMakeTexture(&texture, data->buffer, data->bufferSize);
		if (!texture.data) {
			ASSERT(false);
			return;
		}
		outResource = HandleToResource(_textures.Add(texture));

		_allocator->Deallocate(data->buffer); // @TODO UGLYYYYYYYYYYYYYYYY

		ShaderProgram program;
		program.vertexStage = dependencyParams->dependencies[0].resource;
		program.fragmentStage = dependencyParams->dependencies[1].resource;

		outHandle = HandleToResource(_shaderPrograms.Add(program));*/
	}
}

