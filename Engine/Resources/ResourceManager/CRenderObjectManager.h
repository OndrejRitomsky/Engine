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
	
	class CRenderObjectManager : public IResourceConstructor {
	public:
		CRenderObjectManager();
		~CRenderObjectManager();

		CRenderObjectManager(const CRenderObjectManager& oth) = delete;
		CRenderObjectManager& operator=(const CRenderObjectManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		const RenderObject& GetRenderObject(const Resource& resource) const;

		void RemoveRenderObject(const Resource& resource);

		// ??
		void OnRenderObjectRegister(const ResourceRegisterEvent* data, u32 eventsCount);

	private:
		u32 DependenciesCount(const void* description);
		void FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents);
		void Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle);

	private:
		core::IAllocator* _allocator;

		core::LookupArray<RenderObject> _renderObjects;
	};


}