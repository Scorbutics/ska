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
	class Interpreter;
	class ScriptSleepComponent;

	class ScriptComponent {

	public:
		ScriptComponent() = default;
		ScriptComponent(const ScriptSleepComponent& scriptData, EntityId triggerer, EntityId target, std::unique_ptr<Script> instance);

		float getPriority(const unsigned int currentTimeMillis) const;
		const std::string& name() const { return m_name; }
		bool play(ska::Interpreter& interpreter);

	private:
		ScriptState updateState();
		bool canBePlayed() const;
		void kill() { m_state = ScriptState::DEAD; }

		bool m_deleteEntityWhenFinished = false;
		long m_scriptPeriod = std::numeric_limits<long>::max();
		std::string m_name;
		std::vector<std::string> m_extraArgs;
		ScriptTriggerType m_triggeringType = ScriptTriggerType::NONE;
		std::unique_ptr<Script> m_controller;
		ScriptState m_state = ScriptState::STOPPED;
		unsigned int m_lastTimeStarted = 0;
		unsigned int m_lastTimeDelayed = 0;
		unsigned int m_delay = 0;
		EntityId m_origin = 0;
		EntityId m_target = 0;
	};
}

SKA_DECLARE_COMPONENT(ska::ScriptComponent);
