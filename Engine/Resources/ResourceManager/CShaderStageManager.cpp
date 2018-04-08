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

		_resourceConstructor._DependenciesCount = (IResourceConstructor::DependenciesCountFunction) (&CShaderStageManager::DependenciesCount);
		_resourceConstructor._FillDependencies = (IResourceConstructor::FillDependenciesFunction) (&CShaderStageManager::FillDependencies);
		_resourceConstructor._Create = (IResourceConstructor::CreateFunction) (&CShaderStageManager::Create);
	}

	//---------------------------------------------------------------------------
	IResourceConstructor* CShaderStageManager::ResourceConstructor() {
		return &_resourceConstructor;
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
	u32 CShaderStageManager::DependenciesCount(const ShaderStageDescription* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CShaderStageManager::FillDependencies(const ShaderStageDescription* description, ResourceDependencyEvent* inOutEvents) {
	}

	//---------------------------------------------------------------------------
	void CShaderStageManager::Create(const ShaderStageDescription* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		ShaderStage stage;
		stage.data = description->data;
		outHandle = HandleToResource(_shaderStages.Add(stage));
	}
}

