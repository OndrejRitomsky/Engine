#include "shader_stage_manager.h"

#include <core/common/debug.h>
#include <core/algorithm/cstring.h>
#include <core/collection/lookuparray.inl>
#include <core/collection/hashmap.inl>
#include <core/allocator/allocate.h>

#include "../resource_event.h"

#include "../resources.h"
#include "../resource_util.h"

#include <stddef.h>

namespace eng {
	ResourceTypeFactoryParseFunction(ShaderStageManager::TryCreateShaderStage) {
		const char* buffer = (const char*) resourceDescription;
		u32 size = core::Strlen(buffer);
		ShaderStageManager* manager = (ShaderStageManager*) factory;

		char* data = (char*) Allocate(manager->_allocator, size + 1, sizeof(char));

		core::Memcpy(data, buffer, size + 1);

		ShaderStage stage;
		stage.data = data;

		package->dependenciesCount = 0;
		package->dependant.resource = manager->AddShaderStage(stage);
	}

	ResourceTypeFactoryCreateFunction(ShaderStageManager::CreateShaderStage) {
		ASSERT(false);
	}

	ShaderStageManager::ShaderStageManager() {
		static_assert(offsetof(ShaderStageManager, _factoryInterface) == 0, "Factory interface has to be first");
	}

	ShaderStageManager::~ShaderStageManager() {
	}

	void ShaderStageManager::Init(core::IAllocator* allocator) {
		_shaderStages.Init(allocator, 128);
		_allocator = allocator;
		_factoryInterface.TryCreate = TryCreateShaderStage;
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

