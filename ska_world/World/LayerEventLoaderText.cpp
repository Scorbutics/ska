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

	if (flux.fail()) {
		throw CorruptedFileException("Erreur (classe LayerEvent) : Impossible d'ouvrir le fichier event demande: " + m_fileName);
	}

	std::string line;

	/* Ignore first line */
	getline(flux, line);

	auto i = 0u;
	try {
		while (getline(flux, line)) {
			i++;
			std::stringstream ss;

			BlockEvent event;
			{
                const auto x = StringUtils::template extractTo<std::string>(0, line, ':');
                auto nextIndex = x.size() + 1;

                ss << line.substr(nextIndex);
                int y;
                ss >> y;

                event.position = { StringUtils::strToInt(x), y };
			}

			std::string id;
			ss >> id;
			event.id = (id != "!") ? StringUtils::strToInt(id) : std::numeric_limits<int>::min();

			ss >> event.solid;
			ss >> event.trigger;
			ss >> event.path;
			ss >> event.action;

			getline(ss, event.param);
            event.param = StringUtils::ltrim(event.param);

			events.push_back(std::move(event));
		}
	} catch (NumberFormatException& nfe) {
		throw CorruptedFileException("Erreur (classe LayerEvent) : Erreur lors de la lecture du fichier evenements (ligne : " + StringUtils::uintToStr(i) + ")\n" + std::string(nfe.what()));
	}

	return events;
}

const std::string& ska::LayerEventLoaderText::getName() const {
    return m_fileName;
}
