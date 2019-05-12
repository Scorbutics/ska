#include "LoggerConfig.h"
#include "StateListener.h"

StateListener::StateListener(ska::GameEventDispatcher& ged) : 
	SubObserver<ska::StateEvent>(std::bind(&StateListener::onStateEvent, this, std::placeholders::_1), ged) {
}

const ska::StateEvent* StateListener::getLastStateEventTriggered() const {	
	return m_lastStateEvent.get();
}

bool StateListener::onStateEvent(ska::StateEvent& se) {
	m_lastStateEvent = std::make_unique<ska::StateEvent>(se.type);
	return false;	
}


