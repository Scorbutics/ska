#pragma once
#include <vector>
#include <functional>
#include "BlockEvent.h"

namespace ska {
	struct ScriptGlobalData {
		BlockEvent event;
		ska::ScriptSleepComponent scripts;
	};

	using ScriptGlobalPack = std::vector<ScriptGlobalData>;
	using ScriptGlobalPtrPack = std::vector<std::reference_wrapper<ska::ScriptGlobalData>>;
}