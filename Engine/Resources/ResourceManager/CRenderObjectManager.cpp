#include "CRenderObjectManager.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/LookupArray.inl>
#include <Core/Allocator/IAllocator.h>

#include "Engine/Resources/ResourceEvent.h"

#include "Engine/Resources/Resource/RenderObject.h"
#include "Engine/Resources/Resource/ResourceUtil.h"

namespace eng {



	//---------------------------------------------------------------------------
	CRenderObjectManager::CRenderObjectManager() :
		_allocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CRenderObjectManager::~CRenderObjectManager() {

	}

	//---------------------------------------------------------------------------
	void CRenderObjectManager::Init(core::IAllocator* allocator) {
		_renderObjects.Init(allocator, 128);

		_DependenciesCount = static_cast<IResourceConstructor::DependenciesCountFunction>(&CRenderObjectManager::DependenciesCount);
		_FillDependencies = static_cast<IResourceConstructor::FillDependenciesFunction>(&CRenderObjectManager::FillDependencies);
		_Create = static_cast<IResourceConstructor::CreateFunction>(&CRenderObjectManager::Create);
	}

	//---------------------------------------------------------------------------
	void CRenderObjectManager::RemoveRenderObject(const Resource& resource) {
		_renderObjects.Remove(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	const RenderObject& CRenderObjectManager::GetRenderObject(const Resource& resource) const {
		return _renderObjects.Get(ResourceToHandle(resource));
	}

	//---------------------------------------------------------------------------
	void CRenderObjectManager::OnRenderObjectRegister(const ResourceRegisterEvent* data, u32 eventsCount) {
		for (u32 i = 0; i < eventsCount; ++i) {
			// must be from outside
			/*const ShaderProgramDescription* descr = static_cast<const ShaderProgramDescription*>(data[i].resource);
			Memcpy(&shaderProgram, data[i].resource, sizeof(ShaderProgram));
			_shaderPrograms.Add(data[i].nameHash, shaderProgram);*/

			// todo

			//RenderObjectDescription rod;
		//	Memcpy(&rod, data[i].resource, sizeof(RenderObjectDescription));

		/*	u32 index = _renderObjects.Add(materialTemplate);
			_templatesMap.Add(materialTemplate.Name(), ToResource(index));*/
		}
	}

	//---------------------------------------------------------------------------
	u32 CRenderObjectManager::DependenciesCount(const void* description) {
		return 0;
	}

	//---------------------------------------------------------------------------
	void CRenderObjectManager::FillDependencies(const void* description, ResourceDependencyEvent* inOutEvents) {

	}

	//---------------------------------------------------------------------------
	void CRenderObjectManager::Create(const void* description, const DependencyParams* dependencyParams, Resource& outHandle) {
	//	ASSERT(dependencyParams->dependenciesCount == 2);


		//outHandle = HandleToResource(_shaderPrograms.Add(program));
	}
}

