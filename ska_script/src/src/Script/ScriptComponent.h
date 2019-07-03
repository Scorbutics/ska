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
	class ScriptComponent {
		friend class ScriptRunnerSystem;

	public:
		ScriptComponent() = default;

		bool deleteEntityWhenFinished = false;
		long scriptPeriod = 0;
		std::string name;
		std::vector<std::string> extraArgs;
		ScriptTriggerType triggeringType = ScriptTriggerType::NONE;
		std::unique_ptr<Script> controller;
		int active = 0;

		EntityId getOrigin() const {
			return origin;
		}

		EntityId getTarget() const {
			return target;
		}

	private:
		/* Manage states in order to dispatch all different scripts */
		ScriptState state = ScriptState::STOPPED;
		unsigned int lastTimeStarted = 0;
		unsigned int lastTimeDelayed = 0;
		unsigned int delay = 0;
		EntityId origin = 0;
		EntityId target = 0;
	};
}

SKA_DECLARE_COMPONENT(ska::ScriptComponent);
