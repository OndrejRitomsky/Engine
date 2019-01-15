#pragma once

#include <core/collection/lookuparray.h>
#include <core/collection/hashmap.h>

#include "../resource.h"

#include "iresource_type_factory.h"

namespace core {
	struct IAllocator;
}

namespace eng {
	struct ShaderStage;


	class ShaderStageManager {
	public:
		ShaderStageManager();
		~ShaderStageManager();

		ShaderStageManager(const ShaderStageManager& oth) = delete;
		ShaderStageManager& operator=(const ShaderStageManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		// Internal pointer
		IResourceTypeFactory* Factory();

		Resource AddShaderStage(const ShaderStage& stage);
		const ShaderStage& GetShaderStage(const Resource& resource) const;
		void RemoveShaderStage(const Resource& resource);

	private:
		static ResourceTypeFactoryParseFunction(TryCreateShaderStage);
		static ResourceTypeFactoryCreateFunction(CreateShaderStage);

	private:
		IResourceTypeFactory _factoryInterface;

		core::IAllocator* _allocator = nullptr;
		core::LookupArray<ShaderStage> _shaderStages;
	};
}