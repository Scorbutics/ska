#include <fstream>
#include <sstream>

#include "Utils/StringUtils.h"
#include "LayerEvent.h"
#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/NumberFormatException.h"
#include "Logging/Logger.h"
#include "Utils/FileUtils.h"

ska::LayerEvent::LayerEvent() {
    m_nbrLignes = 0;
}


void ska::LayerEvent::refresh(unsigned int) {
	/* TODO Chipset relative scripts */
}

int ska::LayerEvent::getBlocX(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_coordBX[ligne] : -1;
}

int ska::LayerEvent::getBlocY(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_coordBY[ligne] : -1;
}

int ska::LayerEvent::getID(int ligne) const {
	return (ligne <= m_nbrLignes && ligne >= 0) ? m_ID[ligne] : 0;
}

std::string ska::LayerEvent::getParam(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_param[ligne] : "";
}

int ska::LayerEvent::getTrigger(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_trigger[ligne] : -1;
}

int ska::LayerEvent::getSolide(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_solide[ligne] : -1;
}

std::string ska::LayerEvent::getAction(int ligne) const
{
	if (ligne < m_nbrLignes && ligne >= 0) {
		return m_action[ligne];
	} else if (ligne != 0) {
		SKA_LOG_ERROR("Erreur (classe LayerEvent) : Buffer Overflow lors de la tentative d'accès à la ligne ", ligne, " dans le fichier évènement ", m_nomFichier);
    }
    return "";

}

std::string ska::LayerEvent::getPath(int ligne) const
{
	if (ligne < m_nbrLignes && ligne >= 0) {
		return m_path[ligne];
	} else if (ligne != 0) {
		SKA_LOG_ERROR("Erreur (classe LayerEvent) : Buffer Overflow lors de la tentative d'accès à la ligne ", ligne, " dans le fichier évènement ", m_nomFichier);
    }
    return "";

}

int ska::LayerEvent::getNbrLignes() const {
    return m_nbrLignes;
}

void ska::LayerEvent::changeLevel(const std::string& fichier) {
	ska::FileNameData fndata(fichier);
	m_nomFichier = fichier;
	const auto nomFichier = fndata.name.substr(0, fndata.name.find_last_of('E'));
	std::ifstream flux(fichier.c_str());
	std::stringstream ss;
	if (flux.fail()) {
		throw CorruptedFileException("Erreur (classe LayerEvent) : Impossible d'ouvrir le fichier event demandé: " + fichier);
	}

	m_coordBX.clear();
	m_coordBY.clear();
	m_ID.clear();
	m_trigger.clear();
	m_param.clear();
	m_solide.clear();
	m_action.clear();
	m_path.clear();

	std::string line;

	/* Ignore first line */
	getline(flux, line);

	auto i = 0;
	try {
		while (getline(flux, line)) {

			i++;
			std::size_t nextIndex = 0;
			ss.clear();

			const auto x = StringUtils::extractTo(nextIndex, line, ':');
			m_coordBX.push_back(StringUtils::strToInt(x));
			nextIndex += x.size() + 1;

			ss << line.substr(nextIndex);
			std::string y;
			ss >> y;
			m_coordBY.push_back(StringUtils::strToInt(y));

			std::string id;
			ss >> id;
			if (id != "!") {
				m_ID.push_back(StringUtils::strToInt(id));
			} else {
				m_ID.push_back(std::numeric_limits<int>().min());
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

		}
	} catch (NumberFormatException& nfe) {
		throw CorruptedFileException("Erreur (classe LayerEvent) : Erreur lors de la lecture du fichier evenements (ligne : " + StringUtils::intToStr(i) + ")\n" + std::string(nfe.what()));
	}
	m_nbrLignes = i;
}
