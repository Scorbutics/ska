#include "StateBase.h"

ska::StateBase::StateBase():
	m_state(0), 
	m_active(false) {
}

void ska::StateBase::graphicUpdate(unsigned ellapsedTime, DrawableContainer& drawables){
	onGraphicUpdate(ellapsedTime, drawables);

	/* Graphics */
	for (auto& s : m_subStates) {
		s->graphicUpdate(ellapsedTime, drawables);
	}

	for (auto& s : m_linkedSubStates) {
		s->graphicUpdate(ellapsedTime, drawables);
	}

	for (auto& s : m_graphics) {
		s->setDrawables(drawables);
		s->update(ellapsedTime);
	}	
}

void ska::StateBase::eventUpdate(unsigned int ellapsedTime){
	onEventUpdate(ellapsedTime);

	/* Logics */
	for (auto& s : m_subStates) {
		s->eventUpdate(ellapsedTime);
	}

	for (auto& s : m_linkedSubStates) {
		s->eventUpdate(ellapsedTime);
	}

	for (auto& s : m_logics) {
		s->update(ellapsedTime);
	}
}

void ska::StateBase::load(StatePtr* lastState) {
	m_active = true;
	beforeLoad(lastState);
	m_state = 1;

	for (auto& s : m_subStates) {
		s->load(lastState);
	}

	/*for (auto& s : m_linkedSubStates) {
		s->load(lastState);
	}*/

	m_state = 2;
	afterLoad(lastState);
	m_state = 3;
}

bool ska::StateBase::unload() {
	m_tasks.refresh();
	if (m_state == 3) {
		const auto beforeUnloaded = !beforeUnload();
		if (beforeUnloaded) {
			m_state = 2;
		}
	}

	//If main scene beforeUnload is finished, THEN we can unload subscenes
	if (m_state == 2) {
		const auto wTransitions = waitTransitions();
		if (wTransitions) {
			m_state = 1;
		}
	}

	if (m_state == 1) {
		auto subscenesUnloaded = true;
		for (auto& s : m_subStates) {
			subscenesUnloaded &= !s->unload();
		}

		/*for (auto& s : m_linkedSubStates) {
			subscenesUnloaded &= !s->unload();
		}*/

		if (subscenesUnloaded) {
			m_state = 0;
		}
	}

	//If everything is unloaded, THEN we can call main scene afterUnload
	if (m_state == 0) {
		const auto afterUnloaded = !afterUnload();
		if (afterUnloaded) {
			m_state = -1;
		}
	}

	if (m_state == -1) {
		const auto wTransitions = waitTransitions();
		if (wTransitions) {
			m_state = -2;
			m_active = false;
		}
	}

	return m_state != -2;
}

void ska::StateBase::linkSubState(State& subState) {
	m_linkedSubStates.insert(&subState);
}

void ska::StateBase::unlinkSubState(State& subState){
	m_linkedSubStates.erase(&subState);
}

