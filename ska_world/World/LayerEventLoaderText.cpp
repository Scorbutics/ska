#include <fstream>
#include <sstream>
#include "LayerEventLoaderText.h"
#include "Utils/FileUtils.h"
#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/ExceptionTrigger.h"
#include "Utils/StringUtils.h"
#include "Exceptions/NumberFormatException.h"

ska::LayerEventLoaderText::LayerEventLoaderText(std::string layerFileName) :
	m_fileName(std::move(layerFileName)) {
	FileNameData fndata(m_fileName);

	const auto nomFichier = fndata.name.substr(0, fndata.name.find_last_of('E'));
	std::ifstream flux(m_fileName.c_str());
	if (flux.fail()) {
		ExceptionTrigger<CorruptedFileException>("Erreur (classe LayerEvent) : Impossible d'ouvrir le fichier event demande: " + m_fileName);
	}

	std::string line;

	/* Ignore first line */
	getline(flux, line);

	while (getline(flux, line)) {
		m_fileContent.push_back(line);
	}

}

std::pair<ska::BlockEvent, ska::ScriptSleepComponent> ska::LayerEventLoaderText::buildFromLine(const std::string& line, const unsigned int lineIndex) const {
	std::stringstream ss;
	ss << line;

	ska::BlockEvent event;
	{
		int x;
		ss >> x;

		int y;
		ss >> y;

		event.position = { x, y };
	}

	ss >> event.trigger;

	std::string lineRemaining;
	getline(ss, lineRemaining);
	lineRemaining = lineRemaining.substr(1);

	if (lineRemaining.find_first_of(',') == std::string::npos) {
		event.script = lineRemaining;
		event.param = "" ;
	} else {
		event.script = StringUtils::extractTo<std::string>(0, lineRemaining, ',');
		event.param = StringUtils::trim(lineRemaining.substr(event.script.size() + 1));
	}

	auto ssc = ska::ScriptSleepComponent{};
	std::ifstream currentScript;
	const auto fullName = lineRemaining;

	currentScript.open(event.script, std::ios_base::in);
	if (currentScript.fail()) {
		ExceptionTrigger<ska::FileException>("Script not found : " + event.script + " at line " + ska::StringUtils::intToStr(lineIndex) + " of the level " + m_fileName);
	}

	ssc.id = -1;
	switch (event.trigger) {
	case 0:
		ssc.triggeringType = ska::ScriptTriggerType::AUTO;
		break;
	case 1:
		ssc.triggeringType = ska::ScriptTriggerType::ACTION;
		break;
	case 2:
		ssc.triggeringType = ska::ScriptTriggerType::MOVE_IN;
		break;
	default:
		ssc.triggeringType = ska::ScriptTriggerType::TOUCH;
		break;
	}

	auto args = StringUtils::split(fullName, ',');
	
	ssc.name = args[0];
	
	auto it = args.cbegin();
	for (it++; it != args.cend(); it++) {
		ssc.args.push_back(ska::StringUtils::trim(*it));
	}

	ssc.period = 1000;

	return std::make_pair(event, ssc);
}

ska::Vector2<ska::ScriptPack> ska::LayerEventLoaderText::loadPositioned(unsigned int width, unsigned int height) const {
	auto events = ska::Vector2<ScriptPack>{};
    events.resize(width, height);

	auto i = 0u;
	skaTryCatch(({
		for (const auto& line : m_fileContent) {
			auto [event, ssc] = buildFromLine(line, i++);
			switch(ssc.triggeringType) {
			case ScriptTriggerType::AUTO:
				//Global scripts ignored in this function
				continue;
			default:
				break;
			}
			
			events[event.position.x][event.position.y].push_back(std::move(ssc));
		}
	}), ska::NumberFormatException, nfe, ({
		ExceptionTrigger<CorruptedFileException>("Erreur (classe LayerEvent) : Erreur lors de la lecture du fichier evenements (ligne : " + StringUtils::uintToStr(i) + ")\n" + std::string(nfe.what()));
	}));

	return events;
}

ska::ScriptPack ska::LayerEventLoaderText::loadGlobal() const {
	auto events = ScriptPack{};
	auto i = 0u;
	skaTryCatch (({
		for (const auto& line : m_fileContent) {
			i++;
			
			auto[event, ssc] = buildFromLine(line, i++);
			switch (ssc.triggeringType) {
			case ScriptTriggerType::AUTO:
				break;
			default:
				//Positioned scripts ignored in this function
				continue;
			}

			events.push_back(std::move(ssc));
		}
	}), ska::NumberFormatException, nfe, ({
		ExceptionTrigger<CorruptedFileException>("Erreur (classe LayerEvent) : Erreur lors de la lecture du fichier evenements (ligne : " + StringUtils::uintToStr(i) + ")\n" + std::string(nfe.what()));
	}));
	return events;
}

const std::string& ska::LayerEventLoaderText::getName() const {
    return m_fileName;
}
