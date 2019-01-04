#pragma once

#include <core/collection/lookuparray.h>

#include "iresource_type_factory.h"


namespace eng {
	class MaterialTemplate;	
	struct Material;

	class MaterialManager {
	public:
		MaterialManager();
		~MaterialManager();

		void Init(core::IAllocator* allocator);

		IResourceTypeFactory* Factory();

		IResourceTypeFactory* TemplateFactory();

		const Material& GetMaterial(const Resource& resource) const;
		void RemoveMaterial(const Resource& resource);
		Resource AddMaterial(const Material& program);

		const MaterialTemplate& GetMaterialTemplate(const Resource& resource) const;
		void RemoveMaterialTemplate(const Resource& resource);
		Resource AddMaterialTemplate(const MaterialTemplate& program);


	private:
		static ResourceTypeFactoryParseFunction(ParseMaterial);
		static ResourceTypeFactoryCreateFunction(CreateMaterial);

		static ResourceTypeFactoryParseFunction(ParseMaterialTemplate);
		static ResourceTypeFactoryCreateFunction(CreateMaterialTemplate);


	private:
		IResourceTypeFactory _factoryInterface;
		IResourceTypeFactory _templateFactoryInterface;

		core::IAllocator* _allocator;

		core::LookupArray<MaterialTemplate> _materialTemplates;
		core::LookupArray<Material> _materials;
	};


}