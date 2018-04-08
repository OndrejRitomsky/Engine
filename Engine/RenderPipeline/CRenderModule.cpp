#include "CRenderModule.h"

#include <Core/Common/Assert.h>
#include <Core/Algorithm/Hash.h>

#include <Renderer/API/IRenderDevice.h>
#include <Renderer/GL/GLRenderDevice.h>

#include "Engine/Engine.h"

#include "Engine/Modules/CStaticConstructor.h"

#include "Engine/Memory/CMemoryModule.h"

namespace eng {

	enum class RendererType : u8 {
		OpenGL,
	};

	//---------------------------------------------------------------------------
	CRenderModule::CRenderModule() :
		_state(ModuleState::CREATED),
		_renderDevice(nullptr) {
	}

	//---------------------------------------------------------------------------
	CRenderModule::~CRenderModule() {
		_renderDevice->~IRenderDevice();
	}

	//---------------------------------------------------------------------------
	ModuleState CRenderModule::State() {
		return _state;
	}

	//---------------------------------------------------------------------------
	void CRenderModule::ConstructSubsytems(CStaticConstructor* constructor) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_renderDevice = static_cast<render::IRenderDevice*>(constructor->Construct<render::GLRenderDevice>());
		_rendererType = static_cast<u8>(RendererType::OpenGL);
	}

	//---------------------------------------------------------------------------
	void CRenderModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();
		static_cast<render::GLRenderDevice*>(_renderDevice)->Init(all, all, core::ToHash); // We need type identifier

		_taggedAllocator = engine->memoryModule->TagAllocator();
	}

	//---------------------------------------------------------------------------
	void CRenderModule::Update() {

	}

	//---------------------------------------------------------------------------
	render::IRenderDevice* CRenderModule::RenderDevice() {
		return _renderDevice;
	}
}

