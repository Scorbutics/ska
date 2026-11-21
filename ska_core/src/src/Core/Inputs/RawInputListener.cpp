#include <SDL3/SDL.h>
#include "Core/__internalConfig/LoggerConfig.h"
#include "RawInputListener.h"
#include "../Exceptions/TerminateProcessException.h"
#include "../Exceptions/ExceptionTrigger.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../SDLLibrary.h"

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

	while(SDLLibrary::get().pollEvent(event)) {
		switch (event.type) {
            case SDL_EVENT_TERMINATING:
                /* Terminate the app.
                   Shut everything down before returning from this function.
                */
                break;
            case SDL_EVENT_LOW_MEMORY:
                /* You will get this when your app is paused and iOS wants more memory.
                   Release as much memory as possible.
                */
                break;
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
                /* Prepare your app to go into the background.  Stop loops, etc.
                   This gets called when the user hits the home button, or gets a call.
                */
                break;
            case SDL_EVENT_DID_ENTER_BACKGROUND:
                /* This will get called if the user accepted whatever sent your app to the background.
                   If the user got a phone call and canceled it, you'll instead get an    SDL_APP_DIDENTERFOREGROUND event and restart your loops.
                   When you get this, you have 5 seconds to save all your state or the app will be terminated.
                   Your app is NOT active at this point.
                */
                break;
            case SDL_EVENT_WILL_ENTER_FOREGROUND:
                /* This call happens when your app is coming back to the foreground.
                    Restore all your state here.
                */
                break;
            case SDL_EVENT_DID_ENTER_FOREGROUND:
                /* Restart your loops here.
                   Your app is interactive and getting CPU again.
                */
                break;
            case SDL_EVENT_KEY_DOWN:
                m_keyIn.setKeyState(event.key.scancode, true);
                break;
            case SDL_EVENT_KEY_UP:
				m_keyIn.setKeyState(event.key.scancode, false);
                break;
            case SDL_EVENT_MOUSE_MOTION:
				m_mouseIn.setMousePos(ska::Point<int>(static_cast<int>(event.motion.x), static_cast<int>(event.motion.y)), ska::Point<int>(static_cast<int>(event.motion.xrel), static_cast<int>(event.motion.yrel)));
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
				m_mouseIn.setMouseState(event.button.button, 1);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
				m_mouseIn.setMouseState(event.button.button, 0);
                break;
			case SDL_EVENT_TEXT_EDITING:
				break;
			case SDL_EVENT_TEXT_INPUT:
				m_textInput += StringUtils::toUTF8(event.text.text);
				break;
            case SDL_EVENT_QUIT:
				ExceptionTrigger<TerminateProcessException>("Program quitted");
		default:
                break;
		}

		if(event.type == SDL_EVENT_WINDOW_RESIZED) {
			m_windowInput.setSize(event.window.data1, event.window.data2);
		}
	}

}
