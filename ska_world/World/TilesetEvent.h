#pragma once
#include <unordered_map>
#include "ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	using ScriptTriggerType = char;

	class TilesetEvent {
	public:
		explicit TilesetEvent(std::string chipsetName);
		std::vector<ScriptSleepComponent*> getScript(const std::string& id, const ScriptTriggerType& reason, bool& autoBlackList);
		~TilesetEvent() = default;

		TilesetEvent(TilesetEvent&) = delete;
		void operator=(const TilesetEvent&) = delete;

	private:
		void load();
		void fillScript(const std::string& chipsetFolder, const std::string& id, const ScriptTriggerType& type);

		std::unordered_map<std::string, ScriptSleepComponent> m_triggeredScripts;
		std::unordered_map<int, ScriptSleepComponent> m_autoScripts;

		const std::string m_tilesetName;
	};
}
