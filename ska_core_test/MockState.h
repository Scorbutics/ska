#pragma once
#include "Core/State/State.h"

class MockState : public ska::State {
public:
	explicit MockState(ska::StateHolder& sh);
	~MockState() = default;

	void graphicUpdate(unsigned ellapsedTime, ska::DrawableContainer& drawables) override;
	void eventUpdate(unsigned) override;

	void load(std::unique_ptr<State>* lastState) override;
	bool unload() override;

private:

};
