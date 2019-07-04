#pragma once
#include <vector>
#include <limits>
#include <memory>
#include <unordered_map>
#include "ScriptState.h"
#include "Core/ECS/Basics/Script/ScriptTriggerType.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/ECS/Component.h"

//skalang
#include "Interpreter/Value/Script.h"

namespace ska {
  class ScriptRunnerSystem;
	class Interpreter;
	class ScriptComponent {
		friend class ScriptRunnerSystem;

	public:
		ScriptComponent() = default;

		bool canBePlayed() const;
		float getPriority(const unsigned int currentTimeMillis) const;
		EntityId getOrigin() const { return origin;	}
		EntityId getTarget() const { return target; }
		void kill() { state = ScriptState::DEAD; }
		bool play(ska::Interpreter& interpreter);

	private:
		ScriptState updateFromCurrentTime();

		bool deleteEntityWhenFinished = false;
		long scriptPeriod = std::numeric_limits<long>::max();
		std::string name;
		std::vector<std::string> extraArgs;
		ScriptTriggerType triggeringType = ScriptTriggerType::NONE;
		std::unique_ptr<Script> controller;
		ScriptState state = ScriptState::STOPPED;
		unsigned int lastTimeStarted = 0;
		unsigned int lastTimeDelayed = 0;
		unsigned int delay = 0;
		EntityId origin = 0;
		EntityId target = 0;
	};
}

SKA_DECLARE_COMPONENT(ska::ScriptComponent);
