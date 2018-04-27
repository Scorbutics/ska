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

ska::LayerEvent::LayerEvent(const LayerEventLoaderText& loader, unsigned int width, unsigned int height) :
    m_events(loader.load(width, height)),
    m_fileName(loader.getName()) {
}

ska::LayerEvent::LayerEvent(std::string name, ska::Vector2<std::optional<BlockEvent>> events) :
    m_events(std::move(events)),
    m_fileName(std::move(name)) {
}

void ska::LayerEvent::refresh(unsigned int) {
	/* TODO Tileset relative scripts ?? */
}

void ska::LayerEvent::load(const LayerEventLoader& loader, unsigned int width, unsigned int height) {
    m_events = loader.load(width, height);
    m_fileName = loader.getName();
}
