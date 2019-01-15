#include "material_manager.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/lookuparray.inl>
#include <core/algorithm/hash.h>
#include <core/algorithm/cstring.h>
#include <core/toolbox/json.h>

#include "../resource_event.h"
#include "../template/material_template.h"
#include "../resource_util.h"

#include <stddef.h>


namespace eng {
	static h64 JsonStringToHash(JsonValue* value, const char* key) {
		auto kv = JsonObjectFind(value, key);
		if (!kv || !JsonIsString(&kv->value)) {
			ASSERT(false);
		}

		const char* str = JsonGetString(&kv->value);
		return core::Hash(str, core::Strlen(str));
	}


	ResourceTypeFactoryParseFunction(MaterialManager::TryCreateMaterial) {
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

		ASSERT(path == package->dependant.hash);

		u32 texturesCount;
		auto textureArray = JsonObjectFind(&value, "textures");
		if (!JsonIsArray(&textureArray->value)) {
			ASSERT(false);
		}

		texturesCount = JsonArrayCount(&textureArray->value);

		ASSERT(ResourcePackage::DEPENDENCIES_CAPACITY >= 2 + texturesCount);

		package->dependenciesCount = 2 + texturesCount;
		package->dependencies[0].hash = shader_program;
		package->dependencies[1].hash = material_template;

		package->dependencies[0].typeID = (u64) ResourceType::SHADER_PROGRAM;
		package->dependencies[1].typeID = (u64) ResourceType::MATERIAL_TEMPLATE;

		const JsonValue* texture = JsonArrayCBegin(&textureArray->value);
		const JsonValue* textureEnd = texture + texturesCount;

		for (u32 i = 0; texture < textureEnd; texture++, i++) {
			if (!JsonIsString(texture)) {
				ASSERT(false);
			}
			const char* texturePath = JsonGetString(texture);
			package->dependencies[2 + i].hash = core::Hash(texturePath, (u32) core::Strlen(texturePath));
			package->dependencies[2 + i].typeID = (u64) ResourceType::TEXTURE;
		}

		JsonDeinit(&value);
	}

	ResourceTypeFactoryCreateFunction(MaterialManager::CreateMaterial) {
		MaterialManager* manager = (MaterialManager*) factory;
		ASSERT(package->dependenciesCount >= 2);

		Material material;
		material.shaderProgram = package->dependencies[0].resource;
		material.materialTemplate = package->dependencies[1].resource;
		material.texturesCount = package->dependenciesCount - 2;

		for (u32 i = 0; i < material.texturesCount; ++i) {
			material.textures[i] = package->dependencies[2 + i].resource;
		}

		package->dependant.resource = manager->AddMaterial(material);
	}

	ResourceTypeFactoryParseFunction(MaterialManager::TryCreateMaterialTemplate) {
		ASSERT(ResourcePackage::DEPENDENCIES_CAPACITY >= 1);

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

		ASSERT(path == package->dependant.hash);

		package->dependenciesCount = 1;
		package->dependencies[0].hash = program;
		package->dependencies[0].typeID = (u64) ResourceType::SHADER_PROGRAM;
	}

	ResourceTypeFactoryCreateFunction(MaterialManager::CreateMaterialTemplate) {
		MaterialManager* manager = (MaterialManager*) (factory - 1);
		ASSERT(package->dependenciesCount == 1);

		MaterialTemplate mt;
		mt.Init(package->dependant.hash, package->dependencies[0].hash);
		mt.SetTextures(nullptr, 0);
		mt.SetUniforms(nullptr, nullptr, 0);

		// shader progrma resource where to puit?
		//ASSERT(false);

		package->dependant.resource = manager->AddMaterialTemplate(mt);
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
		_materialTemplates.Init(allocator, 128);
		_allocator = allocator;

		_factoryInterface.TryCreate = TryCreateMaterial;
		_factoryInterface.Create = CreateMaterial;

		_templateFactoryInterface.TryCreate = TryCreateMaterialTemplate;
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

