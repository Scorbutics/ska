#include <fstream>
#include <optional>
#include "TilesetEventLoaderText.h"
#include "Core/Exceptions/FileException.h"
#include "Core/Exceptions/CorruptedFileException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Base/IO/Files/FileUtils.h"

ska::TilesetEventLoaderText::TilesetEventLoaderText(std::string tilesetName) :
	m_tilesetName(std::move(tilesetName)) {
}

std::vector<ska::ScriptSleepComponent> ska::TilesetEventLoaderText::load() const {
	const auto tilesetFolderFn = ska::FileNameData{ m_tilesetName };
	const auto tilesetFolder = tilesetFolderFn.path + "/" + tilesetFolderFn.name;
	std::ifstream scriptList((tilesetFolder + "/scripts.txt").c_str(), std::ifstream::in);
	std::string ss;

	if (scriptList.fail()) {
		ExceptionTrigger<FileException>("Erreur lors de l'ouverture du fichier \"" + tilesetFolder + "/scripts.txt" + "\", fichier de scripts du tileset. ");
	}

	auto scripts = std::vector<ScriptSleepComponent>{};

	while (getline(scriptList, ss)) {
		addScriptIfExists(scripts, fillScript(tilesetFolder, ss, ScriptTriggerType::MOVE_OUT));
		addScriptIfExists(scripts, fillScript(tilesetFolder, ss, ScriptTriggerType::MOVE_IN));
		addScriptIfExists(scripts, fillScript(tilesetFolder, ss, ScriptTriggerType::TOUCH));
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

	auto idSplitter = StringUtils::split(id, '_');
	if (idSplitter.size() != 2) {
		throw ska::CorruptedFileException(("Cannot read the tile event id \"" + id + "\" for the script " + fullName).c_str());
	}

	ScriptSleepComponent ssc;
	ssc.id = ska::Point<int>{ StringUtils::strToInt(idSplitter[0]), StringUtils::strToInt(idSplitter[1])};
	ssc.triggeringType = type;
	ssc.name = fullName;
	ssc.period = 1000;

	return ssc;
}
