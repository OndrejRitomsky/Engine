#include "Window.h"

#include <Extern/Include/SDL/SDL.h>
#include <Extern/Include/GL/glew.h>
#include <Extern/Include/SDL/SDL_opengl.h>

#include <stdio.h>

namespace eng {
	Window::Window() :
		_sdlWindow(nullptr),
		_width(0),
		_height(0) {
	}
	//---------------------------------------------------------------------------

	Window::~Window() {
		SDL_DestroyWindow(_sdlWindow);
		SDL_Quit();
	}
	//---------------------------------------------------------------------------

	bool Window::Init(int majorVersion, int minorVersion, u32 screenWidth, u32 screenHeight, bool vsync) {
		bool success = true;

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			_width = screenWidth;
			_height = screenHeight;

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			_sdlWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
			if (_sdlWindow == NULL) {
				printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				// Can this be here?
				SDL_GLContext sdlGLContext = SDL_GL_CreateContext(_sdlWindow);
				if (sdlGLContext == NULL) {
					printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
					success = false;
				}
				else {
					glewExperimental = GL_TRUE;
					GLenum glewError = glewInit();
					if (glewError != GLEW_OK) {
						printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
					}

					//Use Vsync
					if (SDL_GL_SetSwapInterval(vsync) < 0) {
						printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
					}

					glViewport(0, 0, (GLsizei) screenWidth, (GLsizei) screenHeight);
				}
			}
		}

		return success;
	}
	//---------------------------------------------------------------------------

	void Window::SwapWindowBuffer() {
		SDL_GL_SwapWindow(_sdlWindow);
	}
	//---------------------------------------------------------------------------
}
