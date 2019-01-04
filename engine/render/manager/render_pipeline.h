#pragma once


#include <core/common/types.h>
#include <core/collection/hashmap.h>
#include <core/collection/array.h>

// Maybe we will have separate class for render resources
// And with events they will be sent to register
namespace render {
	struct Texture;
	struct Shader;
}

// @TODO this might be split into the stuff that is handling resources (Update function)
// and second which actualy is render pipeline

namespace eng {
	struct Engine;
	struct Entity;
	struct Frame;
	struct PermanentAllocator;
	struct RenderEntity;
	struct RenderEntityEvents;

	class ResourceModule;
	class WorldModule;
	class HandleManager;

	class RenderPipeline {
	public:
		RenderPipeline();
		~RenderPipeline();

		// Not a module but this will end up having a lot of classes
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		void Update(const Frame* frame, const RenderEntityEvents* entitiesEvent);


	private:
		void OnNewTextureResource(h64 hash);

	private:


		//core::HashMap<render::Texture> _textures;
		
		core::HashMap<render::Shader> _shaders;

		WorldModule* _worldModule;
		ResourceModule* _resourceModule;
		HandleManager* _handleManager;
	};

}

