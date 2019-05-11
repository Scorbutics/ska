#pragma once
#include <unordered_map>
#include <unordered_set>
#include <gsl/pointers>
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/ECS/Basics/Script/ScriptTriggerType.h"
#include "TilesetEventLoader.h"
#include "Core/Point.h"

namespace ska {

	class TilesetEvent : 
		public MovableNonCopyable {
	public:
		explicit TilesetEvent(std::string tilesetName, const TilesetEventLoader& loader);
		TilesetEvent(TilesetEvent&&) = default;

		std::vector<std::reference_wrapper<const ska::ScriptSleepComponent>> getScript(ScriptTriggerType type, const Point<int>& id = Point<int>{}) const;

		~TilesetEvent() override = default;

	private:
		std::string createNameFromScript(ScriptTriggerType type, const Point<int>& id) const;
		void load(const TilesetEventLoader& loader);

		std::unordered_map<std::string, ScriptSleepComponent> m_triggeredScripts;
		std::vector<ScriptSleepComponent> m_autoScripts;

		const std::string m_tilesetName;
	};
}
