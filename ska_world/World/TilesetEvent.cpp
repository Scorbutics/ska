#include <sstream>
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "TilesetEvent.h"
#include "Utils/StringUtils.h"
#include "Point.h"

ska::TilesetEvent::TilesetEvent(std::string tilesetName, const TilesetEventLoader& loader) : 
	m_tilesetName(std::move(tilesetName)) {
	load(loader);
}

std::vector<std::reference_wrapper<const ska::ScriptSleepComponent>> ska::TilesetEvent::getScript(ScriptTriggerType type, const Point<int>& id) const {
	std::vector<std::reference_wrapper<const ska::ScriptSleepComponent>> result;

	if (type == ScriptTriggerType::AUTO) {
		for (auto& s : m_autoScripts) {
			result.emplace_back(s);
		}
	} else {
		const auto fullName = createNameFromScript(type, id);
		if (m_triggeredScripts.find(fullName) != m_triggeredScripts.end()) {
			result.emplace_back(m_triggeredScripts.at(fullName));
		}
	}

	return result;
}

std::string ska::TilesetEvent::createNameFromScript(const ScriptTriggerType type, const Point<int>& id) const {
	std::stringstream ss;
	ss << id.x << "_" << id.y << "_" << static_cast<int>(type);
	const auto fullId = ss.str();
	return m_tilesetName.substr(0, m_tilesetName.find_last_of('.')) + "/Scripts/" + fullId + ".txt";
}

void ska::TilesetEvent::load(const TilesetEventLoader& loader) {
	auto scriptList = loader.load();

	for (auto& s : scriptList) {
		if (s.triggeringType == ScriptTriggerType::AUTO) {
			m_autoScripts.push_back(std::move(s));
		} else {
			m_triggeredScripts.insert(std::make_pair(createNameFromScript(s.triggeringType, s.id), std::move(s)));
		}
	}

}
