#include "Input.h"
/*
#include <Extern/Include/SDL/SDL.h>

namespace eng {

	Input::Input() :
		m_mouseX(0),
		m_mouseY(0),
		m_quit(false) {

		for (int i = 0; i < 322; i++) {
			m_keys[i] = false;
		}
	}

	//----------------------------------------------------------------------------
	Input::~Input() {

	}

	//----------------------------------------------------------------------------
	void Input::UpdateInput() {
		SDL_GetMouseState(&m_mouseX, &m_mouseY);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.key.keysym.sym > 322) continue;

			switch (event.type) {
			case SDL_QUIT:
				m_quit = true;
				break;
			case SDL_KEYDOWN:
				m_keys[event.key.keysym.sym] = true;
				break;
			case SDL_KEYUP:
				m_keys[event.key.keysym.sym] = false;
				break;
			default:
				break;
			}
		}

	}
}
*/