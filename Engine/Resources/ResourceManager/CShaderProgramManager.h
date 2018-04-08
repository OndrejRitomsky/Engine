#pragma once

#include <Core/Collection/LookupArray.h>

#include "Engine/Resources/Resource/Resource.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

namespace core {
	class IAllocator;
}

namespace eng {
	struct ResourceRegisterEvent;
	struct ResourceDependencyEvent;
	struct DependencyParams;

	struct ShaderProgram;
	struct ShaderProgramDescription;
	
	class CShaderProgramManager {
	public:
		CShaderProgramManager();
		~CShaderProgramManager();

		CShaderProgramManager(const CShaderProgramManager& oth) = delete;
		CShaderProgramManager& operator=(const CShaderProgramManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceConstructor* ResourceConstructor();

		const ShaderProgram& GetShaderProgram(const Resource& resource) const;
		
		void RemoveShaderProgram(const Resource& handle);

		// ??
		void OnShaderProgramRegister(const ResourceRegisterEvent* data, u32 eventsCount);

	private:
		// FACTORY 
		u32 DependenciesCount(const ShaderProgramDescription* description);
		void FillDependencies(const ShaderProgramDescription* description, ResourceDependencyEvent* inOutEvents);
		void Create(const ShaderProgramDescription* description, const DependencyParams* dependencyParams, Resource& outHandle);
		// FACTORY 

	private:
		IResourceConstructor _resourceConstructor;

		core::IAllocator* _allocator;

		core::LookupArray<ShaderProgram> _shaderPrograms;
	};



}