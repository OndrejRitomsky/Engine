#pragma once

#include <Core/Collection/LookupArray.h>

#include "Engine/Resources/Resource/Resource.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

namespace eng {
	struct Texture;
	struct TextureDescription;
	struct ResourceDependencyEvent;

	class CTextureManager : public IResourceConstructor {
	public:
		CTextureManager();
		~CTextureManager();

		CTextureManager(const CTextureManager& oth) = delete;
		CTextureManager& operator=(const CTextureManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		const Texture& GetTexture(const Resource& resource) const;

		void RemoveTexture(const Resource& resource);

	private:
		u32 DependenciesCount(const void* description);
		void FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents);
		void Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle);

	private:
		core::IAllocator* _allocator;
		core::LookupArray<Texture> _textures;
	};
}