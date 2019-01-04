#include "platform_module.h"

#include <core/common/debug.h>
#include <core/platform/window.h>

#include "Engine/Engine.h"
#include "engine/memory/permanent_allocator.h"
#include "engine/memory/memory_module.h"

namespace eng {

	PlatformModule::PlatformModule() :
		_state(ModuleState::CREATED),
		_closeRequested(false),
		_windowSizeChanged(false),
		_windowWidth(0),
		_windowHeight(0),
		_window(nullptr) {
	}

	PlatformModule::~PlatformModule() {
		core::WindowDeinit(_window);
	}

	ModuleState PlatformModule::State() {
		return _state;
	}

	void PlatformModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;
	}

	void PlatformModule::Init(Engine* engine, i32 width, i32 height) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* allocator = engine->memoryModule->Allocator();
		_window = core::WindowInit(allocator, width, height, "MyWindow");
		_windowWidth = width;
		_windowHeight = height;

		UpdateWindowEvents(); // Window has first messages in init!
	}

	void PlatformModule::Update(const Frame* frame) {
		ASSERT(_state == ModuleState::OK);

		core::WindowUpdate(_window);

		UpdateWindowEvents();

		{
			u32 count;
			const core::PlatformMousePos* mousePos = WindowGetMousePosEvents(_window, &count);
		}
		
		{
			u32 count;
			const core::PlatformKeyType* input = WindowGetInputEvents(_window, &count);
		}
	}

	void PlatformModule::SwapBackBuffers() {
		core::WindowSwapBuffers(_window);
	}

	void* PlatformModule::DeviceContext() {
		return core::WindowDeviceContext(_window);
	}

	bool PlatformModule::WindowSize(i32* outWidth, i32* outHeight) {
		*outWidth = _windowWidth;
		*outHeight = _windowHeight;

		bool changed = _windowSizeChanged;
		_windowSizeChanged = false;
		return changed;
	}

	bool PlatformModule::CloseRequested() {
		bool close = _closeRequested;
		_closeRequested = false;
		return close;
	}

	void PlatformModule::UpdateWindowEvents() {
		u32 size;
		const char* eventsBuffer = WindowGetEvents(_window, &size);
		const char* end = eventsBuffer + size;

		while (eventsBuffer < end) {
			core::PlatformEventType evt = *(core::PlatformEventType*) eventsBuffer;
			switch (evt) {
			case core::PLATFORM_EVENT_CLOSE:
				_closeRequested = true;
				eventsBuffer += sizeof(core::PlatformEventType);
				break;
			case core::PLATFORM_EVENT_WINDOW_RESIZE:
			{
				core::PlatformEventWindowResize* resize = (core::PlatformEventWindowResize*) eventsBuffer;
				_windowWidth = resize->newWidth;
				_windowHeight = resize->newHeight;
				_windowSizeChanged = true;
				eventsBuffer += sizeof(core::PlatformEventWindowResize);
				break;
			}
			default:
				ASSERT(false);
				break;
			}
		}
	}
}