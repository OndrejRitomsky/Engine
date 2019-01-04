#include "shader_stage_manager.h"

#include <core/common/debug.h>
#include <core/algorithm/cstring.h>
#include <core/collection/lookuparray.inl>
#include <core/collection/hashmap.inl>
#include <core/allocator/allocate.h>

#include "../resource_event.h"

#include "../resource/shader_stage.h"
#include "../resource/resource_util.h"

#include <stddef.h>

namespace eng {
	ResourceTypeFactoryParseFunction(ShaderStageManager::ParseShaderStage) {
		const char* buffer = (const char*) resourceDescription;
		u32 size = core::Strlen(buffer);
		ShaderStageManager* manager = (ShaderStageManager*) factory;

		char* data = (char*) Allocate(manager->_allocator, size, sizeof(char));

		core::Memcpy(data, buffer, size);

		ShaderStage stage;
		stage.data = data;

		manager->_pendingShaderStages.Add(outDependencies->dependant, stage);

		outDependencies->dependenciesCount = 0;
		//ASSERT(outDependencies->dependant == des->hash);
	}

	ResourceTypeFactoryCreateFunction(ShaderStageManager::CreateShaderStage) {
		ASSERT(dependencies->dependenciesCount == 0);

		ShaderStageManager* manager = (ShaderStageManager*) factory;

		ShaderStage* stage = manager->_pendingShaderStages.Find(dependencies->dependant);
		ASSERT(stage);

		ShaderStage newStage = *stage;
		manager->_pendingShaderStages.SwapRemove(dependencies->dependant);

		return manager->AddShaderStage(newStage);
	}


	ShaderStageManager::ShaderStageManager() :
		_allocator(nullptr) {

		static_assert(offsetof(ShaderStageManager, _factoryInterface) == 0, "Factory interface has to be first");
	}

	ShaderStageManager::~ShaderStageManager() {
	}

	void ShaderStageManager::Init(core::IAllocator* allocator) {
		_shaderStages.Init(allocator, 128);
		_pendingShaderStages.Init(allocator);
		_allocator = allocator;
		_factoryInterface.Parse = ParseShaderStage;
		_factoryInterface.Create = CreateShaderStage;
	}

	IResourceTypeFactory* ShaderStageManager::Factory() {
		return &_factoryInterface;
	}

	void ShaderStageManager::RemoveShaderStage(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		Deallocate(_allocator, _shaderStages.Get(handle).data);
		_shaderStages.Remove(handle);
	}

	const ShaderStage& ShaderStageManager::GetShaderStage(const Resource& resource) const {
		return _shaderStages.Get(ResourceToHandle(resource));
	}

	Resource ShaderStageManager::AddShaderStage(const ShaderStage& stage) {
		return HandleToResource(_shaderStages.Add(stage));
	}
}

