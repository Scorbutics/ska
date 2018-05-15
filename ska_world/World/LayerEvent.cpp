#include "LayerEvent.h"
#include "LayerEventLoaderText.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Exceptions/IllegalArgumentException.h"

ska::LayerEvent::LayerEvent(const LayerEventLoader& loader, unsigned int width, unsigned int height) :
    m_fileName(loader.getName()),
    m_events(loader.loadPositioned(width, height)),
	m_autoEvents(loader.loadGlobal()){
	forcesAutoOnGlobalScripts();
}

void ska::LayerEvent::load(const LayerEventLoader& loader, unsigned int width, unsigned int height) {
    m_events = loader.loadPositioned(width, height);
	m_autoEvents = loader.loadGlobal();
    m_fileName = loader.getName();

	forcesAutoOnGlobalScripts();
}

ska::ScriptPack& ska::LayerEvent::getScripts(const Point<int>& coordinates) {
	if(!m_events.has(coordinates.x, coordinates.y)) {
		assert(false && "Coordinates out of bound");
	}
	return m_events[coordinates.x][coordinates.y];
}

ska::ScriptPack& ska::LayerEvent::getAutoScript() {
	return m_autoEvents;
}

void ska::LayerEvent::forcesAutoOnGlobalScripts() {
	for (auto& a : m_autoEvents) {
		a.triggeringType = ScriptTriggerType::AUTO;
	}
}
