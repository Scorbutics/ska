#include <sstream>
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "TilesetEvent.h"
#include "Utils/StringUtils.h"
#include "Point.h"

ska::TilesetEvent::TilesetEvent(std::string tilesetName, const TilesetEventLoader& loader) : 
	m_tilesetName(std::move(tilesetName)) {
	load(loader);
}

std::vector<gsl::not_null<ska::ScriptSleepComponent*>> ska::TilesetEvent::getScript(ScriptTriggerType type, const Point<int>& id) {
	std::vector<gsl::not_null<ScriptSleepComponent*>> result;

	if (type == ScriptTriggerType::AUTO) {
		for (auto& s : m_autoScripts) {
			result.emplace_back(&s.second);
		}
	} else {
		std::stringstream ss;
		ss << id.x << "_" << id.y << "_" << static_cast<char>(type);
		const auto fullId = ss.str();
		const auto fullName = m_tilesetName.substr(0, m_tilesetName.find_last_of('.')) + "/Scripts/" + fullId + ".txt";
		if (m_triggeredScripts.find(fullName) != m_triggeredScripts.end()) {
			result.emplace_back(&m_triggeredScripts.at(fullName));
		}
	}

	return result;
}

void ska::TilesetEvent::load(const TilesetEventLoader& loader) {
	auto scriptList = loader.load();

	for (auto& s : scriptList) {
		if (s.triggeringType == ScriptTriggerType::AUTO) {
			m_autoScripts.insert(std::make_pair(s.id, std::move(s)));
		} else {
			m_triggeredScripts.insert(std::make_pair(s.name, std::move(s)));
		}
	}

}
