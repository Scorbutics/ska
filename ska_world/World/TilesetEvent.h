#pragma once
#include <unordered_map>
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "TilesetEventLoader.h"

namespace ska {
	using ScriptTriggerType = char;

	class TilesetEvent : 
		public MovableNonCopyable {
	public:
		explicit TilesetEvent(std::string tilesetName, const TilesetEventLoader& loader);
		TilesetEvent(TilesetEvent&&) = default;

		std::vector<ScriptSleepComponent*> getScript(const std::string& id, const ScriptTriggerType& reason, bool& autoBlackList);
		~TilesetEvent() override = default;

	private:
		void load(const TilesetEventLoader& loader);

		std::unordered_map<std::string, ScriptSleepComponent> m_triggeredScripts;
		std::unordered_map<int, ScriptSleepComponent> m_autoScripts;

		const std::string m_tilesetName;
	};
}
