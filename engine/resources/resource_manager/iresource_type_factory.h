#pragma once

#include <core/common/types.h>
#include "../resource/resource.h"
#include "../resource_event.h"

namespace eng {

	struct IResourceTypeFactory;


	struct ResourceDependency {
		h64 hash;
		u64 typeID;
		Resource resource;
	};

	struct ResourceDependencies {
		const static u32 DEPENDENCIES_CAPACITY = 10;
		h64 dependant;
		u64 dependenciesCount;
		ResourceDependency dependencies[DEPENDENCIES_CAPACITY];
	};


#define ResourceTypeFactoryParseFunction(NAME) void NAME(IResourceTypeFactory* factory, void* resourceDescription, ResourceDependencies* outDependencies)
#define ResourceTypeFactoryCreateFunction(NAME) Resource NAME(IResourceTypeFactory* factory, const ResourceDependencies* dependencies)

	struct IResourceTypeFactory {
		typedef ResourceTypeFactoryParseFunction(ParseFunction);
		typedef ResourceTypeFactoryCreateFunction(CreateFunction);

		ParseFunction* Parse;
		CreateFunction* Create;
	};
}
