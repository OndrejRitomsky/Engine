#pragma once

#include <Core/Common/Types.h>
#include "Engine/Resources/Resource/Resource.h"

namespace eng {

	struct ResourceDependencyEvent;

	// @TODO rename ?
	struct DependencyParams {
		h64 dependant;
		u64 dependenciesCount;
		const ResourceDependencyEvent* dependencies;
	};

	// @TODO
	// @TODO
	// This has to be changed, when loading + parsing is done (shouldnt pars that twice)
	class IResourceConstructor {
	public:
		u32 DependenciesCount(const void* resourceDescription);

		void FillDependencies(const void* resourceDescription, ResourceDependencyEvent* outEvents);

		void Create(const void* data, const DependencyParams* dependencyParams, void*& outResource);

	public:
		typedef u32(IResourceConstructor::*DependenciesCountFunction)(const void* resourceDescription);
		typedef void(IResourceConstructor::*FillDependenciesFunction)(const void* resourceDescription, ResourceDependencyEvent* outEvents);
		typedef void(IResourceConstructor::*CreateFunction)(const void* resourceDescription, const DependencyParams* dependencyParams, void*& outResource);

		DependenciesCountFunction _DependenciesCount;
		FillDependenciesFunction _FillDependencies;
		CreateFunction _Create;
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------


	//---------------------------------------------------------------------------
	inline u32 IResourceConstructor::DependenciesCount(const void* resourceDescription) {
		return (this->*_DependenciesCount)(resourceDescription);
	}

	//---------------------------------------------------------------------------
	inline void IResourceConstructor::FillDependencies(const void* resourceDescription, ResourceDependencyEvent* outEvents) {
		return (this->*_FillDependencies)(resourceDescription, outEvents);
 	}

	//---------------------------------------------------------------------------
	inline void IResourceConstructor::Create(const void* data, const DependencyParams* dependencyParams, void*& outResource) {
		return (this->*_Create)(data, dependencyParams, outResource);
	}
}
