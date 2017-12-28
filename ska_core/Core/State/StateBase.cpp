#include <algorithm>
#include "StateBase.h"
#include "../../Draw/IGraphicSystem.h"

ska::StateBase::StateBase():
	m_state(0), 
	m_active(false) {
}

void ska::StateBase::graphicUpdate(const unsigned int ellapsedTime, DrawableContainer& drawables){
	onGraphicUpdate(ellapsedTime, drawables);

	/* Graphics */
	for (auto& s : m_subStates) {
		s->graphicUpdate(ellapsedTime, drawables);
	}

	for (auto& s : m_linkedSubStates) {
		s.get().graphicUpdate(ellapsedTime, drawables);
	}

	for (auto& s : m_graphics) {
		s->setDrawables(drawables);
		s->update(ellapsedTime);
	}	
}

void ska::StateBase::eventUpdate(const unsigned int ellapsedTime){
	onEventUpdate(ellapsedTime);

	/* Logics */
	for (auto& s : m_subStates) {
		s->eventUpdate(ellapsedTime);
	}

	for (auto& s : m_linkedSubStates) {
		s.get().eventUpdate(ellapsedTime);
	}

	for (auto& s : m_logics) {
		s->update(ellapsedTime);
	}
}

void ska::StateBase::load(State* lastState) {
	m_active = true;
	beforeLoad(lastState);
	m_state = 1;

	for (auto& s : m_subStates) {
		s->load(lastState);
	}

	m_state = 2;
	afterLoad(lastState);
	m_state = 3;
}

bool ska::StateBase::unload() {
	m_tasks.refresh();
	if (m_state == 3) {
		beforeUnload();
		m_state = 2;
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

		if (subscenesUnloaded) {
			m_state = 0;
		}
	}

	//If everything is unloaded, THEN we can call main scene afterUnload
	if (m_state == 0) {
		afterUnload();
		m_state = -1;
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
	m_linkedSubStates.insert(subState);
}

void ska::StateBase::unlinkSubState(State& subState){
	m_linkedSubStates.erase(subState);
}

void ska::StateBase::transmitLinkedSubstates(StateBase& state){
	m_linkedSubStates = state.m_linkedSubStates;
}

ska::State& ska::StateBase::addSubState(StatePtr s) {
	const auto stateRaw = s.get();
	m_subStates.push_back(std::move(s));

	//manages the case that this current state isn't loaded yet : then substate should be loaded after the state is loaded
	if (m_active) {
		stateRaw->load(nullptr);
	}

	return *stateRaw;
}

bool ska::StateBase::removeSubState(State& subState) {
	auto it = std::remove_if(m_subStates.begin(), m_subStates.end(), [&subState](const auto& c) {
		return c.get() == &subState;
	});

	if(it == m_subStates.end()) {
		return false;
	}

	auto removedState = std::move(*it);
	m_subStates.erase(it, m_subStates.end());
	auto removed = std::move(removedState);
	if (m_active) {
		removed->unload();
	}
	return true;
}

void ska::StateBase::addPriorizedLogic(std::vector<ISystemPtr>& logics, const int priority, ISystemPtr system) const{
	system->setPriority(priority);
	logics.push_back(std::move(system));
	std::sort(logics.begin(), logics.end(), Priorized::comparatorInf<ISystemPtr>);
}

void ska::StateBase::addPriorizedGraphic(std::vector<IGraphicSystemPtr>& graphics, const int priority, IGraphicSystemPtr system) const{
	system->Priorized::setPriority(priority);
	graphics.push_back(std::move(system));
	std::sort(graphics.begin(), graphics.end(), Priorized::comparatorInf<IGraphicSystemPtr>);
}

