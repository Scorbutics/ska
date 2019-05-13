#pragma once
#include <vector>
#include <string>
#include "ScriptTriggerType.h"
#include "../../../Point.h"
#include "Core/ECS/ComponentTag.h"

namespace ska {
	class ScriptSleepComponent {
	public:
		Point<int> id;
		std::string name;
		std::string context;
		ScriptTriggerType triggeringType;
		bool deleteEntityWhenFinished;
		int period;
		std::vector<std::string> args;

		ScriptSleepComponent();
		~ScriptSleepComponent() = default;

	};

	using ScriptPack = std::vector<ScriptSleepComponent>;
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<ScriptSleepComponent>;
#endif
}
