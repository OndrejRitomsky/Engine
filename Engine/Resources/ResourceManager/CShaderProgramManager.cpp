#include "CShaderProgramManager.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/LookupArray.inl>
#include <Core/Allocator/IAllocator.h>

#include "Engine/Resources/ResourceEvent.h"

#include "Engine/Resources/Resource/ShaderProgram.h"
#include "Engine/Resources/Resource/ResourceUtil.h"

namespace eng {
	//---------------------------------------------------------------------------
	CShaderProgramManager::CShaderProgramManager() :
		_allocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CShaderProgramManager::~CShaderProgramManager() {

	}

	//---------------------------------------------------------------------------
	void CShaderProgramManager::Init(core::IAllocator* allocator) {
		_shaderPrograms.Init(allocator, 128);

		_resourceConstructor._DependenciesCount = (IResourceConstructor::DependenciesCountFunction) (&CShaderProgramManager::DependenciesCount);
		_resourceConstructor._FillDependencies = (IResourceConstructor::FillDependenciesFunction) (&CShaderProgramManager::FillDependencies);
		_resourceConstructor._Create = (IResourceConstructor::CreateFunction) (&CShaderProgramManager::Create);
	}

	//---------------------------------------------------------------------------
	IResourceConstructor* CShaderProgramManager::ResourceConstructor() {
		return &_resourceConstructor;
	}

	//---------------------------------------------------------------------------
	void CShaderProgramManager::RemoveShaderProgram(const Resource& resource) {
		_shaderPrograms.Remove(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	const ShaderProgram& CShaderProgramManager::GetShaderProgram(const Resource& resource) const {
		return _shaderPrograms.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	void CShaderProgramManager::OnShaderProgramRegister(const ResourceRegisterEvent* data, u32 eventsCount) {
		for (u32 i = 0; i < eventsCount; ++i) {
			// must be from outside
			/*const ShaderProgramDescription* descr = static_cast<const ShaderProgramDescription*>(data[i].resource);
			Memcpy(&shaderProgram, data[i].resource, sizeof(ShaderProgram));
			_shaderPrograms.Add(data[i].nameHash, shaderProgram);*/

			// todo
		}
	}


	//---------------------------------------------------------------------------
	u32 CShaderProgramManager::DependenciesCount(const ShaderProgramDescription* description) {
		return 2;
	}

	//---------------------------------------------------------------------------
	void CShaderProgramManager::FillDependencies(const ShaderProgramDescription* description, ResourceDependencyEvent* inOutEvents) {
		inOutEvents[0].hash = description->vertexStage;
		inOutEvents[1].hash = description->fragmentStage;

		inOutEvents[0].typeID = static_cast<u64>(ResourceType::SHADER_STAGE);
		inOutEvents[1].typeID = static_cast<u64>(ResourceType::SHADER_STAGE);
	}

	//---------------------------------------------------------------------------
	void CShaderProgramManager::Create(const ShaderProgramDescription* description, const DependencyParams* dependencyParams, Resource& outHandle) {
		ASSERT(dependencyParams->dependenciesCount == 2);

		ShaderProgram program;
		program.vertexStage = dependencyParams->dependencies[0].resource;
		program.fragmentStage = dependencyParams->dependencies[1].resource;

		outHandle = HandleToResource(_shaderPrograms.Add(program));
	}
}

