#pragma once
#include <fakeit.hpp>
#include "Core/State/State.h"

class MockState : public ska::State {
public:
	explicit MockState(fakeit::Mock<ska::State>& instance);
	~MockState() = default;

	void graphicUpdate(unsigned ellapsedTime, ska::DrawableContainer& drawables) override;
	void eventUpdate(unsigned) override;

	void loadBefore(ska::State* lastState) override;
	void loadAfter(ska::State* lastState) override;
	bool unload() override;

private:
	fakeit::Mock<ska::State>& m_instance;
};
