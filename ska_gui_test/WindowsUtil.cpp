#include <doctest.h>
#include "WindowsUtil.h"
#include "Core/Window.h"
#include <Data/Events/GameEventDispatcher.h>
#include "Inputs/InputContextManager.h"
#include "InputContextTest.h"

ska::GameEventDispatcher& GetGED() {
	static ska::GameEventDispatcher ged;
	return ged;
}

ska::RawInputListener& GetRawInputListener() {
	static ska::RawInputListener ril;
	return ril;
}

InputContextTest* SetICT(InputContextTest* ict) {
	static InputContextTest* rIct = ict;
	return rIct;
}

//TODO meilleure conception pour éviter de dépendre d'une fenêtre
void GetWindow(){
	static auto initialized = false;
	if (!initialized) {
		static ska::Window window("ska_gui_test", 100, 100);
		ska::SDLRenderer::setDefaultRenderer(window.getRenderer().getDefaultRenderer());
		initialized = true;
	}
}

InputContextTest* GetICT() {
	GetICM();
	GetWindow();
    return SetICT(nullptr);
}

ska::InputContextManager& GetICM() {
	static ska::InputContextManager playerICM(GetRawInputListener(), GetGED());
	static auto initialized = false;
	if(!initialized) {
		initialized = true;
		auto ict = std::make_unique<InputContextTest>();
		SetICT(ict.get());
		playerICM.addContext(ska::CONTEXT_GUI, std::move(ict));
	}
	return playerICM;
}
