#pragma once

#include <core/collection/lookuparray.h>

#include "../resource.h"

#include "iresource_type_factory.h"

namespace core {
	struct IAllocator;
}

namespace eng {
	struct ShaderProgram;


	class ShaderProgramManager {
	public:
		ShaderProgramManager();
		~ShaderProgramManager();

		ShaderProgramManager(const ShaderProgramManager& oth) = delete;
		ShaderProgramManager& operator=(const ShaderProgramManager& rhs) = delete;

		void Init(core::IAllocator* allocator);

		IResourceTypeFactory* Factory();

		Resource AddShaderProgram(const ShaderProgram& program);
		const ShaderProgram& GetShaderProgram(const Resource& resource) const;
		void RemoveShaderProgram(const Resource& handle);

	private:
		static ResourceTypeFactoryParseFunction(TryCreateProgram);
		static ResourceTypeFactoryCreateFunction(CreateProgram);

	private:
		IResourceTypeFactory _factoryInterface;
		core::LookupArray<ShaderProgram> _shaderPrograms;
	};
	
}