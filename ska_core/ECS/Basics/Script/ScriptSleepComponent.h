#pragma once
#include <vector>
#include <string>
#include "ScriptTriggerType.h"

namespace ska {
	class ScriptSleepComponent {
	public:
		int id;
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
}
