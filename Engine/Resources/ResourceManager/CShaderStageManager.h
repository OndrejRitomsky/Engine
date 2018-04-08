#pragma once

#include <Core/Collection/LookupArray.h>

#include "Engine/Resources/Resource/Resource.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

namespace core {
	class IAllocator;
}

namespace eng {
	struct ResourceDescriptionLoadedEvent;
	struct ShaderStage;
	struct DependencyParams;
	
	struct ShaderStageDescription;

	class CShaderStageManager {
	public:
		CShaderStageManager();
		~CShaderStageManager();

		CShaderStageManager(const CShaderStageManager& oth) = delete;
		CShaderStageManager& operator=(const CShaderStageManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceConstructor* ResourceConstructor();

		const ShaderStage& GetShaderStage(const Resource& resource) const;

		void RemoveShaderStage(const Resource& resource);

	private:
		// FACTORY 
		u32 DependenciesCount(const ShaderStageDescription* description);
		void FillDependencies(const ShaderStageDescription* description, ResourceDependencyEvent* inOutEvents);
		void Create(const ShaderStageDescription* description, const DependencyParams* dependencyParams, Resource& outHandle);
		// FACTORY 

	private:
		IResourceConstructor _resourceConstructor;
		core::IAllocator* _allocator;
		core::LookupArray<ShaderStage> _shaderStages;
	};


}