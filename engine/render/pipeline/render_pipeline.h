#pragma once


#include <core/common/types.h>
#include <core/collection/hashmap.h>
#include <core/collection/array.h>

// @TODO will be moved when camera is created
#include <core/math/m44.h>

namespace render {
	class RenderContextStream;
	class IRenderDevice;
	class RenderContext;

	struct ResourceContext;
}

namespace core {
	struct IAllocator;
}

// @TODO this might be split into the stuff that is handling resources (Update function)
// and second which actualy is render pipeline

namespace eng {
	struct Engine;
	struct Frame;
	struct Entity;
	struct PermanentAllocator;
	struct RenderObjectComponent;
	struct RenderObject;

	class ResourceModule;
	class WorldModule;
	class HandleManager;
	class RenderObjectComponentManager;


	struct PipelineShader;
	struct PipelineMesh;
	struct PipelineObject;  // @TODO this should be saved and stored so its not rebuild every time... PipelineObject defined by its Resource... 
	struct PipelineTexture;

	struct Mesh;


	class RenderPipeline {
	public:
		RenderPipeline();
		~RenderPipeline();

		// Not a module but this will end up having a lot of classes
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		void Render(render::IRenderDevice* renderDevice, const Frame* frame, const RenderObjectComponent* components, u32 componentsCount);

		void SetViewPort(i32 width, i32 height);

	private:
		void CreatePipelineObject(const Frame* frame, render::RenderContextStream* stream, const RenderObject* renderObject, PipelineObject* outPipelineObject);

		void RenderPipelineObject(render::RenderContext* context, const PipelineObject* pipelineObject, const Entity* entity);

		// could be cleaner
		void RegisterPipelineMesh(const Frame* frame, render::RenderContextStream* stream, const Mesh* mesh, PipelineMesh* outPipelineMesh);

	private:
		bool _viewportChanged = false;
		i32 _width = 1024;
		i32 _height = 768;

		M44 _viewMatrix; //@TODO camera

		core::IAllocator* _allocator = nullptr;
		HandleManager* _handleManager = nullptr;
		ResourceModule* _resourceModule = nullptr;
		WorldModule* _worldModule = nullptr;
		RenderObjectComponentManager* _renderObjectComponentManager = nullptr;

		core::HashMap<PipelineShader> _shaders;
		core::HashMap<PipelineMesh> _meshes;
		core::HashMap<PipelineTexture> _textures;
		core::HashMap<PipelineObject> _objects;

		core::Array<render::ResourceContext> _newResources; // this shouldnt be in array since it will be used by another thread (@TODO tag allocate)
	};

}

