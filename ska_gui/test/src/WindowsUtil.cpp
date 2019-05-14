#include "GUI/__internalConfig/LoggerConfig.h"
#include "WindowsUtil.h"
#include <Core/Data/Events/GameEventDispatcher.h>
#include "Core/Inputs/InputContextManager.h"
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

MockRenderer& MakeMockRenderer() {
	static MockRenderer mr;
	return mr;
}

InputContextTest* GetICT() {
	GetICM();
    return SetICT(nullptr);
}

ska::InputContextManager& GetICM() {
	static ska::InputContextManager playerICM(GetRawInputListener(), GetGED());
	static auto initialized = false;
	if(!initialized) {
		initialized = true;
		auto ict = std::make_unique<InputContextTest>();
		SetICT(ict.get());
		playerICM.addContext(ska::EnumContextManager::CONTEXT_GUI, std::move(ict));
	}
	return playerICM;
}
