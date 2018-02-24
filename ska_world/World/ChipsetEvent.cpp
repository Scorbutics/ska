#include <fstream>
#include <SDL.h>
#include "Exceptions/FileException.h"
#include "Utils/StringUtils.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "ChipsetEvent.h"
#include "Utils/SkaConstants.h"

ska::ChipsetEvent::ChipsetEvent(std::string chipsetName) : 
	m_chipsetName(std::move(chipsetName)) {
	load();
}

void ska::ChipsetEvent::load() {
    const auto& chipsetFolder = m_chipsetName.substr(0, m_chipsetName.find_last_of('.'));
	std::ifstream scriptList((chipsetFolder + "" FILE_SEPARATOR "scripts.txt").c_str(), std::ifstream::in);
	std::string ss;

	if (scriptList.fail()) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + chipsetFolder + "" FILE_SEPARATOR "scripts.txt" + "\", fichier de scripts du chipset. " + std::string(SDL_GetError()));
	}

	while (getline(scriptList, ss)) {
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::TOUCH);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::MOVE_OUT);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::MOVE_IN);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::ACTION);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::AUTO);
	}

}

void ska::ChipsetEvent::fillScript(const std::string& chipsetFolder, const std::string& id, const ScriptTriggerType& type) {
	std::ifstream currentScript;
	const std::string fullName = chipsetFolder + "" FILE_SEPARATOR "Scripts" FILE_SEPARATOR "" + id + "_" + (char)(type + '0') + ".txt";
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

std::vector<ska::ScriptSleepComponent*> ska::ChipsetEvent::getScript(const std::string& id, const ScriptTriggerType& reason, bool& autoBlackList) {
	std::vector<ScriptSleepComponent*> result;

	if (reason == EnumScriptTriggerType::AUTO && !autoBlackList) {
		for (auto& s : m_autoScripts) {
			result.push_back(&s.second);
		}
		autoBlackList = true;
	} else if(!id.empty()) {
		const auto fullId = id + "_" + static_cast<char>(reason + '0');
		const auto fullName = m_chipsetName.substr(0, m_chipsetName.find_last_of('.')) + "" FILE_SEPARATOR "Scripts" FILE_SEPARATOR "" + fullId + ".txt";
		if (m_triggeredScripts.find(fullName) != m_triggeredScripts.end()) {
			result.push_back(&m_triggeredScripts.at(fullName));
		}
	}

	return result;
}
