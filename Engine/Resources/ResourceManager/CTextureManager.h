#pragma once

#include <Core/Collection/LookupArray.h>

#include "Engine/Resources/Resource/Resource.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

namespace eng {
	struct Texture;
	struct TextureDescription;
	struct ResourceDependencyEvent;

	class CTextureManager {
	public:
		CTextureManager();
		~CTextureManager();

		CTextureManager(const CTextureManager& oth) = delete;
		CTextureManager& operator=(const CTextureManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceConstructor* ResourceConstructor();

		const Texture& GetTexture(const Resource& resource) const;

		void RemoveTexture(const Resource& resource);

	private:
		// FACTORY 
		u32 DependenciesCount(const TextureDescription* description);
		void FillDependencies(const TextureDescription* description, ResourceDependencyEvent* inOutEvents);
		void Create(const TextureDescription* description, const DependencyParams* dependencyParams, Resource& outHandle);
		// FACTORY 

	private:
		IResourceConstructor _resourceConstructor;
		core::IAllocator* _allocator;
		core::LookupArray<Texture> _textures;
	};


}