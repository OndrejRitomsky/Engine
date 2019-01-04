#include "material_manager.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/lookuparray.inl>
#include <core/algorithm/hash.h>
#include <core/algorithm/cstring.h>
#include <core/toolbox/json.h>

#include "../resource_event.h"
#include "../resource/material.h"
#include "../resource/template/material_template.h"
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


	ResourceTypeFactoryParseFunction(MaterialManager::ParseMaterial) {
		char* text = (char*) resourceDescription;

		JsonValue value;
		if (!JsonParse(text, nullptr, &value)) {
			ASSERT(false);
		}

		if (!JsonIsObject(&value)) {
			ASSERT(false);
		}

		h64 path = JsonStringToHash(&value, "path");
		h64 shader_program = JsonStringToHash(&value, "shader_program");
		h64 material_template = JsonStringToHash(&value, "material_template");
		u32 texturesCount;

		{
			auto kv = JsonObjectFind(&value, "textures");
			if (!JsonIsArray(&kv->value)) {
				ASSERT(false);
			}

			texturesCount = JsonArrayCount(&kv->value);
			ASSERT(texturesCount == 0);
		}

		JsonDeinit(&value);
		ASSERT(path == outDependencies->dependant);


		ASSERT(ResourceDependencies::DEPENDENCIES_CAPACITY >= 2 + texturesCount);

		outDependencies->dependenciesCount = 2;
		outDependencies->dependencies[0].hash = shader_program;
		outDependencies->dependencies[1].hash = material_template;

		outDependencies->dependencies[0].typeID = (u64) ResourceType::SHADER_PROGRAM;
		outDependencies->dependencies[1].typeID = (u64) ResourceType::MATERIAL_TEMPLATE;
	}

	ResourceTypeFactoryCreateFunction(MaterialManager::CreateMaterial) {
		MaterialManager* manager = (MaterialManager*) factory;
		ASSERT(dependencies->dependenciesCount == 2);

		Material material;
		material.shaderProgram = dependencies->dependencies[0].resource;
		material.materialTemplate = dependencies->dependencies[1].resource;
		material.textures = nullptr;
		material.texturesCount = 0;

		return manager->AddMaterial(material);
	}

	ResourceTypeFactoryParseFunction(MaterialManager::ParseMaterialTemplate) {
		ASSERT(ResourceDependencies::DEPENDENCIES_CAPACITY >= 1);

		char* text = (char*) resourceDescription;

		JsonValue value;
		if (!JsonParse(text, nullptr, &value)) {
			ASSERT(false);
		}

		if (!JsonIsObject(&value)) {
			ASSERT(false);
		}

		h64 path = JsonStringToHash(&value, "path");
		h64 program = JsonStringToHash(&value, "shader_program");


		JsonDeinit(&value);

		ASSERT(path == outDependencies->dependant);

		outDependencies->dependenciesCount = 1;
		outDependencies->dependencies[0].hash = program;
		outDependencies->dependencies[0].typeID = (u64) ResourceType::SHADER_PROGRAM;
	}

	ResourceTypeFactoryCreateFunction(MaterialManager::CreateMaterialTemplate) {
		MaterialManager* manager = (MaterialManager*) (factory - 1);
		ASSERT(dependencies->dependenciesCount == 1);

		MaterialTemplate mt;
		mt.Init(dependencies->dependant, dependencies->dependencies[0].hash);
		mt.SetTextures(nullptr, 0);
		mt.SetUniforms(nullptr, nullptr, 0);


		// shader progrma resource where to puit?
		ASSERT(false);

		return manager->AddMaterialTemplate(mt);
	}



	MaterialManager::MaterialManager() :
		_allocator(nullptr) {
		static_assert(offsetof(MaterialManager, _factoryInterface) == 0, "Factory interface has to be first");
		static_assert(offsetof(MaterialManager, _templateFactoryInterface) == sizeof(IResourceTypeFactory), "Factory interface has to be first");
	}

	MaterialManager::~MaterialManager() {
	}

	void MaterialManager::Init(core::IAllocator* allocator) {
		_materials.Init(allocator, 128);
		_allocator = allocator;

		_factoryInterface.Parse = ParseMaterial;
		_factoryInterface.Create = CreateMaterial;

		_templateFactoryInterface.Parse = ParseMaterialTemplate;
		_templateFactoryInterface.Create = CreateMaterialTemplate;
	}

	IResourceTypeFactory* MaterialManager::Factory() {
		return &_factoryInterface;
	}

	IResourceTypeFactory* MaterialManager::TemplateFactory() {
		return &_templateFactoryInterface;
	}

	const Material& MaterialManager::GetMaterial(const Resource& resource) const {
		return _materials.Get(ResourceToHandle(resource));
	}

	void MaterialManager::RemoveMaterial(const Resource& resource) {
		core::Handle handle = ResourceToHandle(resource);
		//_allocator->Deallocate(_materials.Get(handle).textures);
		_materials.Remove(handle);
	}

	Resource MaterialManager::AddMaterial(const Material& program) {
		return HandleToResource(_materials.Add(program));
	}

	const MaterialTemplate& MaterialManager::GetMaterialTemplate(const Resource& resource) const {
		return _materialTemplates.Get(ResourceToHandle(resource));
	}

	void MaterialManager::RemoveMaterialTemplate(const Resource& resource) {
		_materialTemplates.Remove(ResourceToHandle(resource));
	}

	Resource MaterialManager::AddMaterialTemplate(const MaterialTemplate& program) {
		return HandleToResource(_materialTemplates.Add(program));
	}
}

