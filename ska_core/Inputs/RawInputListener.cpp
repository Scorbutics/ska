#include <SDL.h>
#include "RawInputListener.h"
#include "../Exceptions/TerminateProcessException.h"
#include "../Utils/StringUtils.h"

const ska::KeyInput& ska::RawInputListener::getKeyInput() const {
    return m_keyIn;
}

const ska::MouseInput& ska::RawInputListener::getMouseInput() const {
	return m_mouseIn;
}

const std::wstring& ska::RawInputListener::getTextInput() const {
	return m_textInput;
}

const ska::WindowInput& ska::RawInputListener::getWindowInput() const {
	return m_windowInput;
}

void ska::RawInputListener::update() {
    SDL_Event event;
	m_mouseIn.setMouseLastPos(m_mouseIn.getMousePos());

	m_textInput = L"";
	m_keyIn.resetTriggers();
	m_mouseIn.resetTriggers();

	while(SDL_PollEvent(&event)) {
		switch (event.type) {
            case SDL_APP_TERMINATING:
                /* Terminate the app.
                   Shut everything down before returning from this function.
                */
                break;
            case SDL_APP_LOWMEMORY:
                /* You will get this when your app is paused and iOS wants more memory.
                   Release as much memory as possible.
                */
                break;
            case SDL_APP_WILLENTERBACKGROUND:
                /* Prepare your app to go into the background.  Stop loops, etc.
                   This gets called when the user hits the home button, or gets a call.
                */
                break;
            case SDL_APP_DIDENTERBACKGROUND:
                /* This will get called if the user accepted whatever sent your app to the background.
                   If the user got a phone call and canceled it, you'll instead get an    SDL_APP_DIDENTERFOREGROUND event and restart your loops.
                   When you get this, you have 5 seconds to save all your state or the app will be terminated.
                   Your app is NOT active at this point.
                */
                break;
            case SDL_APP_WILLENTERFOREGROUND:
                /* This call happens when your app is coming back to the foreground.
                    Restore all your state here.
                */
                break;
            case SDL_APP_DIDENTERFOREGROUND:
                /* Restart your loops here.
                   Your app is interactive and getting CPU again.
                */
                break;
            case SDL_KEYDOWN:
                m_keyIn.setKeyState(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
				m_keyIn.setKeyState(event.key.keysym.scancode, false);
                break;
            case SDL_MOUSEMOTION:
				m_mouseIn.setMousePos(ska::Point<int>(event.motion.x, event.motion.y), ska::Point<int>(event.motion.xrel, event.motion.yrel));
                break;
            case SDL_MOUSEBUTTONDOWN:
				m_mouseIn.setMouseState(event.button.button, 1);
                break;
            case SDL_MOUSEBUTTONUP:
				m_mouseIn.setMouseState(event.button.button, 0);
                break;
			case SDL_TEXTEDITING:
				break;
			case SDL_TEXTINPUT:
				m_textInput += StringUtils::toUTF8(event.edit.text);
				break;
            case SDL_QUIT:
				throw TerminateProcessException("Program quitted");
		default:
                break;
		}

		if(event.type == SDL_WINDOWEVENT) {
			switch(event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					m_windowInput.setSize(event.window.data1, event.window.data2);
					break;

				default:
					break;
			}
		}
	}

}
