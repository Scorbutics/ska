#pragma once
#include <unordered_map>
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "TilesetEventLoader.h"
#include "Point.h"
#include <gsl/pointers>

namespace ska {

	class TilesetEvent : 
		public MovableNonCopyable {
	public:
		explicit TilesetEvent(std::string tilesetName, const TilesetEventLoader& loader);
		TilesetEvent(TilesetEvent&&) = default;

		std::vector<gsl::not_null<ScriptSleepComponent*>> getScript(ScriptTriggerType type, const Point<int>& id = Point<int>{});

		~TilesetEvent() override = default;

	private:
		void load(const TilesetEventLoader& loader);

		std::unordered_map<std::string, ScriptSleepComponent> m_triggeredScripts;
		std::unordered_map<int, ScriptSleepComponent> m_autoScripts;

		const std::string m_tilesetName;
	};
}
