#pragma once

#include <Core/Common/Types.h>

struct SDL_Window;

// @TODO REWORK 

namespace eng {
	class Window {
	public:
		Window();
		~Window();

		bool Init(int majorVersion, int minorVersion, u32 screenWidth, u32 screenHeight, bool vsync);
		void SwapWindowBuffer();

		inline int Width();
		inline int Height();

	private:
		u32 _width; 
		u32 _height;

		SDL_Window* _sdlWindow;
		//SDL_GLContext _sdlGLContext; // @TODO might want more renderers
	};



	inline int Window::Width() {
		return _width;
	}

	inline int Window::Height() {
		return _height;
	}

}