#include "CShaderStageManager.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/LookupArray.inl>
#include <Core/Allocator/IAllocator.h>

#include "Engine/Resources/ResourceEvent.h"

#include "Engine/Resources/Resource/ShaderStage.h"
#include "Engine/Resources/Resource/ResourceUtil.h"

namespace eng {
	//---------------------------------------------------------------------------
	CShaderStageManager::CShaderStageManager() :
		_allocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CShaderStageManager::~CShaderStageManager() {
	}

	//---------------------------------------------------------------------------
	void CShaderStageManager::Init(core::IAllocator* allocator) {
		_shaderStages.Init(allocator, 128);

		_allocator = allocator;

		_DependenciesCount = static_cast<IResourceConstructor::DependenciesCountFunction>(&CShaderStageManager::DependenciesCount);
		_FillDependencies = static_cast<IResourceConstructor::FillDependenciesFunction>(&CShaderStageManager::FillDependencies);
		_Create = static_cast<IResourceConstructor::CreateFunction>(&CShaderStageManager::Create);
	}

	//---------------------------------------------------------------------------
	void CShaderStageManager::RemoveShaderStage(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		_allocator->Deallocate(_shaderStages.Get(handle).data); // @TODO this is utterly disgusting, do tagged 
		_shaderStages.Remove(handle);
	}

	//---------------------------------------------------------------------------
	const ShaderStage& CShaderStageManager::GetShaderStage(const Resource& resource) const {
		return _shaderStages.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	u32 CShaderStageManager::DependenciesCount(const void* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CShaderStageManager::FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents) {
	}

	//---------------------------------------------------------------------------
	void CShaderStageManager::Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		const ShaderStageDescription* des = static_cast<const ShaderStageDescription*>(description);
		ShaderStage stage;
		stage.data = des->data;
		outHandle = HandleToResource(_shaderStages.Add(stage));
	}
}

