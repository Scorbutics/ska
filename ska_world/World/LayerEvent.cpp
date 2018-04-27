#include "LayerEvent.h"
#include "LayerEventLoaderText.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"

ska::LayerEvent::LayerEvent(const LayerEventLoader& loader, unsigned int width, unsigned int height) :
    m_fileName(loader.getName()),
    m_events(loader.loadPositioned(width, height)),
	m_autoEvents(loader.loadGlobal()){
}

void ska::LayerEvent::load(const LayerEventLoader& loader, unsigned int width, unsigned int height) {
    m_events = loader.loadPositioned(width, height);
	m_autoEvents = loader.loadGlobal();
    m_fileName = loader.getName();
}

ska::ScriptPack& ska::LayerEvent::getScript(const ska::Point<int>& coordinates) {
	return m_events[coordinates.x][coordinates.y];
}

ska::ScriptPack& ska::LayerEvent::getAutoScript() {
	return m_autoEvents;
}
