#pragma once


#include <Core/Common/Types.h>
#include <Core/Collection/HashMap.h>


namespace core {
	class IAllocator;
	class ITagAllocator;
}

// Maybe we will have separate class for render resources
// And with events they will be sent to register
namespace render {
	struct Texture;
	struct Shader;
}

namespace eng {
	class CResourceModule;
	class CRenderComponentManager;
	class CHandleManager;
	class CStaticConstructor;
	
	struct Entity;

	class CRenderPipeline {
	public:
		CRenderPipeline();
		~CRenderPipeline();

		// Not a module but this will end up having a lot of classes
		void ConstructSubsytems(CStaticConstructor* constructor);
		
		//  maybe there is nicer way to do this
		void Init(core::IAllocator* allocator, core::ITagAllocator* taggedAllocator,
			CResourceModule* resourceModule, CRenderComponentManager* renderObjectManager);
		

		//void OnNewRenderObjectEvent(Entity* entity, u32 count);
		
		void Render(Entity* entity, u32 count);

	private:
		void OnNewTextureResource(h64 hash);

	private:
		core::HashMap<render::Texture> _textures;
		core::HashMap<render::Shader> _shaders;

		core::IAllocator* _allocator;
		core::ITagAllocator* _taggedAllocator;

		CResourceModule* _resourceModule;
		CRenderComponentManager* _renderComponentManager;

		CHandleManager* _handleManager;
	};

}

