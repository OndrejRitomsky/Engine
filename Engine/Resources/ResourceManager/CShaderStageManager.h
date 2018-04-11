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

	class CShaderStageManager : public IResourceConstructor {
	public:
		CShaderStageManager();
		~CShaderStageManager();

		CShaderStageManager(const CShaderStageManager& oth) = delete;
		CShaderStageManager& operator=(const CShaderStageManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		const ShaderStage& GetShaderStage(const Resource& resource) const;

		void RemoveShaderStage(const Resource& resource);

	private:
		u32 DependenciesCount(const void* description);
		void FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents);
		void Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle);

	private:
		core::IAllocator* _allocator;
		core::LookupArray<ShaderStage> _shaderStages;
	};


}