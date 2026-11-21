#include <SDL3/SDL.h>
#include "KeyboardInputGUIContext.h"

//TODO move to an SDL specific module
ska::KeyboardInputGUIContext::KeyboardInputGUIContext(ska::RawInputListener& ril) : ska::KeyboardInputContext(ril) {
}

void ska::KeyboardInputGUIContext::buildCodeMap(std::unordered_map<int, InputAction>& codeMap, std::unordered_map<int, InputToggle>& toggles) {
	
	codeMap[SDL_SCANCODE_ESCAPE] = Quit;
	codeMap[SDL_SCANCODE_RETURN] = DoAction;

	codeMap[SDL_SCANCODE_BACKSPACE] = DeleteChar;

	/* Mouse click */
	/* Left */
	codeMap[SDL_SCANCODE_COUNT + 0] = LClic;
	toggles[SDL_SCANCODE_COUNT + 0] = MoveWindow;

	/* Right */
	codeMap[SDL_SCANCODE_COUNT + 1] = RClic;
}

ska::KeyboardInputGUIContext::~KeyboardInputGUIContext() {
}
