#include "shader_program_manager.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/lookuparray.inl>
#include <core/algorithm/hash.h>
#include <core/algorithm/cstring.h>
#include <core/toolbox/json.h>

#include "../resource_event.h"
#include "../resource/shader_program.h"
#include "../resource/resource_util.h"

#include <stddef.h>



namespace eng {

	static h64 JsonStringToHash(JsonValue* value, const char* key) {
		auto kv = JsonObjectFind(value, key);
		if (!JsonIsString(&kv->value)) {
			ASSERT(false);
		}

		const char* str = JsonGetString(&kv->value);
		return core::Hash(str, core::Strlen(str));
	}


	ResourceTypeFactoryParseFunction(ShaderProgramManager::ParseProgram) {
		ASSERT(ResourceDependencies::DEPENDENCIES_CAPACITY >= 2);

		char* text = (char*) resourceDescription;

		JsonValue value;
		if (!JsonParse(text, nullptr, &value)) {
			ASSERT(false);
		}

		if (!JsonIsObject(&value)) {
			ASSERT(false);
		}

		h64 path = JsonStringToHash(&value, "path");
		h64 vertex = JsonStringToHash(&value, "vertex_stage");
		h64 fragment = JsonStringToHash(&value, "fragment_stage");

		JsonDeinit(&value);

		ASSERT(path == outDependencies->dependant);

		outDependencies->dependenciesCount = 2;
		outDependencies->dependencies[0].hash = vertex;
		outDependencies->dependencies[1].hash = fragment;

		outDependencies->dependencies[0].typeID = (u64) ResourceType::SHADER_STAGE;
		outDependencies->dependencies[1].typeID = (u64) ResourceType::SHADER_STAGE; 
	}

	ResourceTypeFactoryCreateFunction(ShaderProgramManager::CreateProgram) {
		ShaderProgramManager* manager = (ShaderProgramManager*) factory;
		ASSERT(dependencies->dependenciesCount == 2);

		ShaderProgram program;
		program.vertexStage = dependencies->dependencies[0].resource;
		program.fragmentStage = dependencies->dependencies[1].resource;

		return manager->AddShaderProgram(program);
	}


	ShaderProgramManager::ShaderProgramManager() {
		static_assert(offsetof(ShaderProgramManager, _factoryInterface) == 0, "Factory interface has to be first");
	}


	ShaderProgramManager::~ShaderProgramManager() {

	}


	void ShaderProgramManager::Init(core::IAllocator* allocator) {
		_shaderPrograms.Init(allocator, 128);
		_factoryInterface.Parse = ParseProgram;
		_factoryInterface.Create = CreateProgram;
	}

	IResourceTypeFactory* ShaderProgramManager::Factory() {
		return &_factoryInterface;
	}


	void ShaderProgramManager::RemoveShaderProgram(const Resource& resource) {
		_shaderPrograms.Remove(ResourceToHandle(resource));
	}


	const ShaderProgram& ShaderProgramManager::GetShaderProgram(const Resource& resource) const {
		return _shaderPrograms.Get(ResourceToHandle(resource));
	}


	Resource ShaderProgramManager::AddShaderProgram(const ShaderProgram& program) {
		return HandleToResource(_shaderPrograms.Add(program));
	}
}

