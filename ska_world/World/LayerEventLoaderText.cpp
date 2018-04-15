#include <fstream>
#include <sstream>
#include "LayerEventLoaderText.h"
#include "Utils/FileUtils.h"
#include "Exceptions/CorruptedFileException.h"
#include "Utils/StringUtils.h"
#include "Exceptions/NumberFormatException.h"

ska::LayerEventLoaderText::LayerEventLoaderText(std::string layerFileName) :
	m_fileName(std::move(layerFileName)) {
}

std::vector<ska::BlockEvent> ska::LayerEventLoaderText::load() const {
	FileNameData fndata(m_fileName);

	auto events = std::vector<BlockEvent>{};
	
	const auto nomFichier = fndata.name.substr(0, fndata.name.find_last_of('E'));
	std::ifstream flux(m_fileName.c_str());
	std::stringstream ss;
	if (flux.fail()) {
		throw CorruptedFileException("Erreur (classe LayerEvent) : Impossible d'ouvrir le fichier event demande: " + m_fileName);
	}

	std::string line;

	/* Ignore first line */
	getline(flux, line);

	auto i = 0;
	try {
		while (getline(flux, line)) {
			BlockEvent event;
			
			i++;
			std::size_t nextIndex = 0;
			ss.clear();

			const auto x = StringUtils::extractTo(nextIndex, line, ':');
			nextIndex += x.size() + 1;

			ss << line.substr(nextIndex);
			int y;
			ss >> y;

			event.position = { x, y };

			std::string id;
			ss >> id;
			if (id != "!") {
				m_ID.push_back(StringUtils::strToInt(id));
			} else {
				m_ID.push_back(std::numeric_limits<int>::min());
			}

			std::string solide;
			ss >> solide;
			m_solide.push_back(StringUtils::strToInt(solide));

			std::string trigger;
			ss >> trigger;
			m_trigger.push_back(StringUtils::strToInt(trigger));

			std::string path;
			ss >> path;
			m_path.push_back(path);

			std::string action;
			ss >> action;
			m_action.push_back(action);

			std::string param;
			getline(ss, param);
			m_param.push_back(StringUtils::ltrim(param));

			events.push_back(std::move(event));
		}
	} catch (NumberFormatException& nfe) {
		throw CorruptedFileException("Erreur (classe LayerEvent) : Erreur lors de la lecture du fichier evenements (ligne : " + StringUtils::intToStr(i) + ")\n" + std::string(nfe.what()));
	}
	m_nbrLignes = i;

	return events;
}
