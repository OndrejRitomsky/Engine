#pragma once

#include <Core/Collection/LookupArray.h>

#include "Engine/Resources/Resource/Resource.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

namespace core {
	class IAllocator;
}

namespace eng {
	struct ResourceLoadedEvent;
	struct ResourceRegisterEvent;
	struct RenderObjectDescription;

	struct RenderObject;
	
	class CRenderObjectManager {
	public:
		CRenderObjectManager();
		~CRenderObjectManager();

		CRenderObjectManager(const CRenderObjectManager& oth) = delete;
		CRenderObjectManager& operator=(const CRenderObjectManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceConstructor* ResourceConstructor();

		const RenderObject& GetRenderObject(const Resource& resource) const;

		void RemoveRenderObject(const Resource& resource);

		// ??
		void OnRenderObjectRegister(const ResourceRegisterEvent* data, u32 eventsCount);

	private:
		// FACTORY 
		u32 DependenciesCount(const RenderObjectDescription* description);
		void FillDependencies(const RenderObjectDescription* description, ResourceDependencyEvent* inOutEvents);
		void Create(const RenderObjectDescription* description, const DependencyParams* dependencyParams, Resource& outHandle);
		// FACTORY 

	private:
		IResourceConstructor _resourceConstructor;

		core::IAllocator* _allocator;

		core::LookupArray<RenderObject> _renderObjects;
	};


}