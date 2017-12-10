#include <doctest.h>
#include "Core/State/StateHolder.h"
#include "Exceptions/StateDiedException.h"
#include "StateListener.h"
#include "MockState.h"


TEST_CASE("[StateHolder]"){
	ska::GameEventDispatcher ged;

	SUBCASE("First state loaded") {
		ska::StateHolder stateHolder(ged);
		StateListener stateListener(ged);

		CHECK(stateListener.getLastStateEventTriggered() == nullptr);
		
		//First state
		stateHolder.nextState(std::make_unique<MockState>(stateHolder));
		stateHolder.update();

		auto lastEvent = stateListener.getLastStateEventTriggered();
		CHECK(lastEvent != nullptr);
		CHECK(lastEvent->type == ska::FIRST_STATE_LOADED);
	}

	SUBCASE("State change") {
		ska::StateHolder stateHolder(ged);
		StateListener stateListener(ged);

		//First state
		stateHolder.nextState(std::make_unique<MockState>(stateHolder));
		stateHolder.update();

		//Next state
		stateHolder.nextState(std::make_unique<MockState>(stateHolder));
		try {
			stateHolder.update();
			CHECK(false);
		} catch (ska::StateDiedException& sde) {
			auto lastEvent = stateListener.getLastStateEventTriggered();
			CHECK(lastEvent->type == ska::STATE_CHANGED);
		}
	}
}
