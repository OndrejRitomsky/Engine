#pragma once

#include "../module.h"

#include "resource_event.h" // @CLEANUP,  split, leaking?  


namespace core {
	struct ITagAllocator;
}

namespace eng {
	struct Engine;
	struct Frame;
	struct PermanentAllocator;

	class TextureManager;
	class ShaderStageManager;
	class ShaderProgramManager;
	class MaterialManager;
	class MeshManager;
	class RenderObjectManager;

	class ResourcesManager;
	class ResourcesFactory;


	// @TODO resource releasing acquiring counting
	// @TODO acknowledge loads to loader (which were caused by factory)

	// @TODO Cleaning managers since its lookup array it has to be iterated from top
	//           But manager doesnt know about type!

	class ResourceModule {
	private:
		struct Context;

	public:
		ResourceModule();
		~ResourceModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		void Update(const Frame* frame);
		void OnEventsByType(const Frame* frame, const void* events, ResourceEventType eventsType);
		void QueryEventsByType(ResourceEventType type, void* outEvents);
		void ClearPendingEvents();

		ResourcesManager* GetResourcesManager();

		const RenderObjectManager* GetRenderObjectManager();
		const MeshManager* GetMeshManager();
		const TextureManager* GetTextureManager();
		const MaterialManager* GetMaterialManager();
		const ShaderStageManager* GetShaderStageManager();
		const ShaderProgramManager* GetShaderProgramManager();


	private:
		ModuleState _state = ModuleState::CREATED;

		ResourcesManager* _resourcesManager = nullptr;
		ResourcesFactory* _resourcesFactory = nullptr;

		RenderObjectManager* _renderObjectManager = nullptr;
		MeshManager* _meshManager = nullptr;
		TextureManager* _textureManager = nullptr;
		MaterialManager* _materialManager = nullptr;

		ShaderStageManager* _shaderStageManager = nullptr;
		ShaderProgramManager* _shaderProgramManager = nullptr;
	};
}