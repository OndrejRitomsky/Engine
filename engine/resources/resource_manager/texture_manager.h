#pragma once

#include <core/collection/lookuparray.h>

#include "../resource/resource.h"



namespace eng {
	struct Texture;
	struct TextureDescription;
	struct ResourceDependencyEvent;

	/*class TextureManager : public IResourceConstructor {
	public:
		TextureManager();
		~TextureManager();

		TextureManager(const TextureManager& oth) = delete;
		TextureManager& operator=(const TextureManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		const Texture& GetTexture(const Resource& resource) const;

		void RemoveTexture(const Resource& resource);

	private:
		u32 DependenciesCount(const void* description);
		void FillDependencies(const void* description, ResourceDependencyEvent* outEvents);
		void Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle);

	private:
		core::IAllocator* _allocator;
		core::LookupArray<Texture> _textures;
	};*/
}