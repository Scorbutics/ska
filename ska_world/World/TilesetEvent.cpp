#include <fstream>
#include "Exceptions/FileException.h"
#include "Utils/StringUtils.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "TilesetEvent.h"

ska::TilesetEvent::TilesetEvent(std::string tilesetName) : 
	m_tilesetName(std::move(tilesetName)) {
	load();
}

void ska::TilesetEvent::load() {
    const auto& tilesetFolder = m_tilesetName.substr(0, m_tilesetName.find_last_of('.'));
	std::ifstream scriptList((tilesetFolder + "/scripts.txt").c_str(), std::ifstream::in);
	std::string ss;

	if (scriptList.fail()) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + tilesetFolder + "/scripts.txt" + "\", fichier de scripts du tileset. ");
	}

	while (getline(scriptList, ss)) {
		fillScript(tilesetFolder, ss, EnumScriptTriggerType::TOUCH);
		fillScript(tilesetFolder, ss, EnumScriptTriggerType::MOVE_OUT);
		fillScript(tilesetFolder, ss, EnumScriptTriggerType::MOVE_IN);
		fillScript(tilesetFolder, ss, EnumScriptTriggerType::ACTION);
		fillScript(tilesetFolder, ss, EnumScriptTriggerType::AUTO);
	}

}

void ska::TilesetEvent::fillScript(const std::string& chipsetFolder, const std::string& id, const ScriptTriggerType& type) {
	std::ifstream currentScript;
	const auto fullName = chipsetFolder + "/Scripts/" + id + "_" + static_cast<char>(type + '0') + ".txt";
	currentScript.open(fullName, std::ios_base::in);
	if (currentScript.fail()) {
		return;
	}

	ScriptSleepComponent ssc;
	ssc.triggeringType = type;
	ssc.name = fullName;
	ssc.period = 1000;

	if (type == EnumScriptTriggerType::AUTO) {
		m_autoScripts.insert(std::make_pair(StringUtils::strToInt(id), ssc));
	} else {
		m_triggeredScripts.insert(make_pair(fullName, ssc));
	}

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
