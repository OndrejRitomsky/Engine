#pragma once

#include <core/common/types.h>
#include "../resource.h"
#include "../resource_event.h"

namespace eng {
	struct IResourceTypeFactory;

	struct ResourceDescription {
		h64 hash;
		u64 typeID;
		Resource resource;
	};

	struct ResourcePackage {
		const static u32 DEPENDENCIES_CAPACITY = 10; // might be small number for textures?
		u32 dependenciesCount;
		ResourceDescription dependant;
		ResourceDescription dependencies[DEPENDENCIES_CAPACITY];
	};

	// if outDependencies.dependenciesCount = 0 then Resource is created and valid and there is no need to call Create
	// TryCreate -> dependencies if there are none then fill package
	// Create    -> dependencies are given fill package

#define ResourceTypeFactoryParseFunction(NAME)  void NAME(IResourceTypeFactory* factory, void* resourceDescription, u32 resourceDescriptionSize, ResourcePackage* package)
#define ResourceTypeFactoryCreateFunction(NAME) void NAME(IResourceTypeFactory* factory, ResourcePackage* package)

	struct IResourceTypeFactory {
		typedef ResourceTypeFactoryParseFunction(TryCreateFunction);
		typedef ResourceTypeFactoryCreateFunction(CreateFunction);

		TryCreateFunction* TryCreate;
		CreateFunction* Create;
	};
}
