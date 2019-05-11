#pragma once
#include <memory>
#include "Core/Data/Events/StateEvent.h"
#include "Core/Data/Events/GameEventDispatcher.h"
#include "Base/Patterns/SubObserver.h"

class StateListener :
	public ska::SubObserver<ska::StateEvent> {
public:
	explicit StateListener(ska::GameEventDispatcher& ged);

	const ska::StateEvent* getLastStateEventTriggered() const;

private:
	bool onStateEvent(ska::StateEvent& se);

	std::unique_ptr<ska::StateEvent> m_lastStateEvent;
};
