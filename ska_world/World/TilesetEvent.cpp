#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "TilesetEvent.h"
#include "Utils/StringUtils.h"

ska::TilesetEvent::TilesetEvent(std::string tilesetName, const TilesetEventLoader& loader) : 
	m_tilesetName(std::move(tilesetName)) {
	load(loader);
}

std::vector<ska::ScriptSleepComponent*> ska::TilesetEvent::getScript(const std::string& id, const ScriptTriggerType& reason, bool& autoBlackList) {
	std::vector<ScriptSleepComponent*> result;

	if (reason == EnumScriptTriggerType::AUTO && !autoBlackList) {
		for (auto& s : m_autoScripts) {
			result.push_back(&s.second);
		}
		autoBlackList = true;
	} else if(!id.empty()) {
		const auto fullId = id + "_" + static_cast<char>(reason + '0');
		const auto fullName = m_tilesetName.substr(0, m_tilesetName.find_last_of('.')) + "/Scripts/" + fullId + ".txt";
		if (m_triggeredScripts.find(fullName) != m_triggeredScripts.end()) {
			result.push_back(&m_triggeredScripts.at(fullName));
		}
	}

	return result;
}

void ska::TilesetEvent::load(const TilesetEventLoader& loader) {
	auto scriptList = loader.load();

	for (auto& s : scriptList) {
		if (s.triggeringType == EnumScriptTriggerType::AUTO) {
			m_autoScripts.insert(std::make_pair(s.id, std::move(s)));
		} else {
			m_triggeredScripts.insert(std::make_pair(s.name, std::move(s)));
		}
	}

}
