#include "Draw/DrawableContainer.h"
#include "MockState.h"

MockState::MockState(fakeit::Mock<ska::State>& instance) :
	m_instance(instance) {

	fakeit::Fake(Method(m_instance, graphicUpdate));
	fakeit::Fake(Method(m_instance, eventUpdate));
	fakeit::Fake(Method(m_instance, load));
	Method(m_instance, unload) = false;
}

void MockState::graphicUpdate(unsigned ellapsedTime, ska::DrawableContainer& drawables) {
	return m_instance().graphicUpdate(ellapsedTime, drawables);
}

void MockState::eventUpdate(unsigned ellapsedTime) {
	return m_instance().eventUpdate(ellapsedTime);
}

void MockState::load(ska::StatePtr* lastState) {
	return m_instance().load(lastState);
}

bool MockState::unload() {
	return m_instance().unload();
}


