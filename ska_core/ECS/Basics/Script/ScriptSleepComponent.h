#pragma once
#include <vector>
#include "../../SerializableComponent.h"
#include "ScriptTriggerType.h"

namespace ska {
	class ScriptSleepComponent : public SerializableComponent {
	public:
		std::string name;
		std::string context;
		ScriptTriggerType triggeringType;
		bool deleteEntityWhenFinished;
		int period;
		std::vector<std::string> args;

		ScriptSleepComponent();
		~ScriptSleepComponent() = default;

	};
}
