#include <fstream>
#include <sstream>
#include <limits>

#include "Utils/StringUtils.h"
#include "LayerEvent.h"
#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/NumberFormatException.h"
#include "Logging/Logger.h"
#include "LayerEventLoaderText.h"
#include "Utils/FileUtils.h"

ska::LayerEvent::LayerEvent(const LayerEventLoaderText& loader) :
    m_events(loader.load()),
    m_fileName(loader.getName()) {
}

void ska::LayerEvent::refresh(unsigned int) {
	/* TODO Tileset relative scripts ?? */
}

int ska::LayerEvent::getBlocX(int ligne) const {
	return (ligne < m_events.size() && ligne >= 0) ? m_events[ligne].position.x : -1;
}

int ska::LayerEvent::getBlocY(int ligne) const {
	return (ligne < m_events.size() && ligne >= 0) ? m_events[ligne].position.y : -1;
}

int ska::LayerEvent::getID(int ligne) const {
	return (ligne <= m_events.size() && ligne >= 0) ? m_events[ligne].id : 0;
}

std::string ska::LayerEvent::getParam(int ligne) const {
	return (ligne < m_events.size() && ligne >= 0) ? m_events[ligne].param : "";
}

int ska::LayerEvent::getTrigger(int ligne) const {
	return (ligne < m_events.size() && ligne >= 0) ? m_events[ligne].trigger : -1;
}

int ska::LayerEvent::getSolid(int ligne) const {
	return (ligne < m_events.size() && ligne >= 0) ? m_events[ligne].solid : -1;
}

std::string ska::LayerEvent::getAction(int ligne) const {
	if (ligne < m_events.size() && ligne >= 0) {
		return m_events[ligne].action;
	}

	if (ligne != 0) {
		SKA_LOG_ERROR("Erreur (classe LayerEvent) : Buffer Overflow lors de la tentative d'accès à la ligne ", ligne, " dans le fichier évènement ", m_fileName);
    }
    return "";

}

std::string ska::LayerEvent::getPath(int ligne) const {
	if (ligne < m_events.size() && ligne >= 0) {
		return m_events[ligne].path;
	}

	if (ligne != 0) {
		SKA_LOG_ERROR("Erreur (classe LayerEvent) : Buffer Overflow lors de la tentative d'accès à la ligne ", ligne, " dans le fichier évènement ", m_fileName);
    }
    return "";

}

int ska::LayerEvent::getLines() const {
    return m_events.size();
}

void ska::LayerEvent::load(const LayerEventLoader& loader) {
    m_events = loader.load();
    m_fileName = loader.getName();
}
