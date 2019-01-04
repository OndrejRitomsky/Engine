#pragma once

#include <core/collection/lookuparray.h>

#include "../resource/resource.h"
#include "iresource_type_factory.h"

namespace core {
	struct IAllocator;
}

namespace eng {
	struct RenderObject;

	class RenderObjectManager {
	public:
		RenderObjectManager();
		~RenderObjectManager();

		RenderObjectManager(const RenderObjectManager& oth) = delete;
		RenderObjectManager& operator=(const RenderObjectManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceTypeFactory* Factory();

		const RenderObject& GetRenderObject(const Resource& resource) const;
		void RemoveRenderObject(const Resource& resource);
		Resource AddRenderObject(const RenderObject& program);


	private:
		static ResourceTypeFactoryParseFunction(ParseRenderObject);
		static ResourceTypeFactoryCreateFunction(CreateRenderObject);

	private:
		IResourceTypeFactory _factoryInterface;
		//core::IAllocator* _allocator;
		core::LookupArray<RenderObject> _renderObjects;
	};

}