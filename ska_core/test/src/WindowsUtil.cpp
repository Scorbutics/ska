#include "Core/__internalConfig/LoggerConfig.h"
#include "Core/Data/Events/GameEventDispatcher.h"
#include "WindowsUtil.h"
#include "Core/Inputs/InputContextManager.h"

ska::GameEventDispatcher& GetGED() {
	static ska::GameEventDispatcher ged;
	return ged;
}

ska::RawInputListener& GetRawInputListener() {
	static ska::RawInputListener ril;
	return ril;
}

MockRenderer& MakeMockRenderer() {
	static MockRenderer mr;
	return mr;
}
