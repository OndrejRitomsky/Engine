#pragma once

#include "Engine/Modules/ModuleState.h"

#include "Engine/Resources/ResourceEvent.h" // @CLEANUP,  split, leaking?  


namespace core {
	class ITagAllocator;
}

namespace eng {
	struct Engine;
	struct Frame;

	class CStaticConstructor;

	class CTextureManager;
	class CShaderStageManager;
	class CShaderProgramManager;
	class CMaterialManager;
	class CMeshManager;
	class CRenderObjectManager;

	class CResourcesManager;
	class CResourcesFactory;
	class CResourcesLoader;


	// @TODO resource releasing acquiring counting
	// @TODO acknowledge loads to loader (which were caused by factory)

	// @TODO Cleaning managers since its lookup array it has to be iterated from top
	//           But manager doesnt know about type!

	class CResourceModule {
	private:
		struct Context;

	public:
		CResourceModule();
		~CResourceModule();

		ModuleState State();
		void ConstructSubsytems(CStaticConstructor* constructor);
		void Init(Engine* engine);

		void Update(const Frame* frame);

		void OnEventsByType(const void* events, ResourceEventType eventsType);

		void QueryEventsByType(ResourceEventType type, void* outEvents);

		void ClearPendingEvents();

		CResourcesLoader* ResourcesLoader();
		CResourcesManager* ResourcesManager();

		const CRenderObjectManager* RenderObjectManager();
		const CMeshManager* MeshManager();
		const CTextureManager* TextureManager();
		const CShaderStageManager* ShaderStageManager();
		const CShaderProgramManager* ShaderProgramManager();
		const CMaterialManager* MaterialManager();

	private:
		void UpdateResourcesFactoryEvents(const Frame* frame);

	private:
		ModuleState _state;

		CResourcesLoader* _resourcesLoader;
		CResourcesManager* _resourcesManager;
		CResourcesFactory* _resourcesFactory;

		CRenderObjectManager* _renderObjectManager;
		CMeshManager* _meshManager;
		CTextureManager* _textureManager;
		CShaderStageManager* _shaderStageManager;
		CShaderProgramManager* _shaderProgramManager;
		CMaterialManager* _materialManager;

		core::ITagAllocator* _tagAllocator;
	};
}