#include <fstream>
#include <optional>
#include "TilesetEventLoaderText.h"
#include "Exceptions/FileException.h"
#include "Utils/StringUtils.h"

ska::TilesetEventLoaderText::TilesetEventLoaderText(std::string tilesetName) :
	m_tilesetName(std::move(tilesetName)) {
}

std::vector<ska::ScriptSleepComponent> ska::TilesetEventLoaderText::load() const {
	const auto& tilesetFolder = m_tilesetName.substr(0, m_tilesetName.find_last_of('.'));
	std::ifstream scriptList((tilesetFolder + "/scripts.txt").c_str(), std::ifstream::in);
	std::string ss;

	if (scriptList.fail()) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + tilesetFolder + "/scripts.txt" + "\", fichier de scripts du tileset. ");
	}

	auto scripts = std::vector<ScriptSleepComponent>{};

	while (getline(scriptList, ss)) {
		addScriptIfExists(scripts, fillScript(tilesetFolder, ss, ScriptTriggerType::MOVE_OUT));
		addScriptIfExists(scripts, fillScript(tilesetFolder, ss, ScriptTriggerType::MOVE_IN));
		addScriptIfExists(scripts, fillScript(tilesetFolder, ss, ScriptTriggerType::ACTION));
		addScriptIfExists(scripts, fillScript(tilesetFolder, ss, ScriptTriggerType::AUTO));
	}

	return scripts;
}

void ska::TilesetEventLoaderText::addScriptIfExists(std::vector<ska::ScriptSleepComponent>& scripts, std::optional<ska::ScriptSleepComponent> s) {
	if (s.has_value()) {
		scripts.push_back(s.value());
	}
}


std::optional<ska::ScriptSleepComponent> ska::TilesetEventLoaderText::fillScript(const std::string& chipsetFolder, const std::string& id, const ScriptTriggerType& type) {
	std::ifstream currentScript;
	const auto fullName = chipsetFolder + "/Scripts/" + id + "_" + static_cast<char>(static_cast<char>(type) + '0') + ".txt";
	currentScript.open(fullName, std::ios_base::in);
	if (currentScript.fail()) {
		return std::optional<ScriptSleepComponent>();
	}

	ScriptSleepComponent ssc;
	ssc.id = StringUtils::strToInt(id);
	ssc.triggeringType = type;
	ssc.name = fullName;
	ssc.period = 1000;

	return ssc;
}
