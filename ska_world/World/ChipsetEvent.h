#pragma once
#include <unordered_map>
#include "ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	using ScriptTriggerType = char;

	class ChipsetEvent {
	public:
		explicit ChipsetEvent(std::string chipsetName);
		std::vector<ScriptSleepComponent*> getScript(const std::string& id, const ScriptTriggerType& type, bool& autoBlackList);
		~ChipsetEvent() = default;

		ChipsetEvent(ChipsetEvent&) = delete;
		void operator=(const ChipsetEvent&) = delete;

	private:
		void load();
		void fillScript(const std::string& chipsetFolder, const std::string& id, const ScriptTriggerType& type);

		std::unordered_map<std::string, ScriptSleepComponent> m_triggeredScripts;
		std::unordered_map<int, ScriptSleepComponent> m_autoScripts;

		const std::string m_chipsetName;

		uint32_t m_darkColor;
		uint32_t m_whiteColor;
		uint32_t m_lightColor;
	};
}
