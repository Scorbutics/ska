#include "WindowsUtil.h"
#include <Data/Events/GameEventDispatcher.h>
#include "Inputs/InputContextManager.h"

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
