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
    class ScriptAutoSystem;
	class ScriptComponent {
		friend class ScriptAutoSystem;

	public:
		ScriptComponent():
			deleteEntityWhenFinished(false),
			scriptPeriod(0),
			triggeringType(ScriptTriggerType::NONE),
			origin(0),
			target(0) {
			state = EnumScriptState::STOPPED;
			lastTimeStarted = 0;
			commandsPlayed = 0;
			lastTimeDelayed = 0;
			delay = 0;
			active = 0;
			parent = nullptr;
		}

		bool deleteEntityWhenFinished;
		long scriptPeriod;
		std::string context;
		std::string extendedName;
		std::vector<std::string> extraArgs;
		ScriptTriggerType triggeringType;
		ScriptAutoSystem* parent;
		std::unique_ptr<Script> controller;
		int active;
		std::unordered_map<std::string, std::string> varMap;
		std::string fullPath;
		std::string key;

		EntityId getOrigin() const {
			return origin;
		}

		EntityId getTarget() const {
			return target;
		}

	private:
		/* Manage states in order to dispatch all different scripts */
		ScriptState state;
		unsigned int lastTimeStarted;
		unsigned int commandsPlayed;
		std::string lastResult;
		unsigned int lastTimeDelayed;
		unsigned int delay;
		EntityId origin;
		EntityId target;
	};
}

SKA_DECLARE_COMPONENT(ska::ScriptComponent);
