#include <doctest.h>
#include "Core/State/StateHolder.h"
#include "Exceptions/StateDiedException.h"
#include "StateListener.h"
#include "MockState.h"

TEST_CASE("[StateHolder]"){
	using namespace fakeit;

	ska::GameEventDispatcher ged;

	SUBCASE("First state loaded") {
		ska::StateHolder stateHolder(ged);
		StateListener stateListener(ged);

		CHECK(stateListener.getLastStateEventTriggered() == nullptr);
		
		Mock<ska::State> mockState;

		//First state
		stateHolder.nextState(std::make_unique<MockState>(mockState));
		stateHolder.update();

		auto lastEvent = stateListener.getLastStateEventTriggered();
		CHECK(lastEvent != nullptr);
		CHECK(lastEvent->type == ska::FIRST_STATE_LOADED);

		Verify(Method(mockState, load));
	}

	SUBCASE("State change") {
		ska::StateHolder stateHolder(ged);
		StateListener stateListener(ged);

		Mock<ska::State> mockState;
		//First state
		stateHolder.nextState(std::make_unique<MockState>(mockState));
		stateHolder.update();

		Verify(Method(mockState, load));

		Mock<ska::State> mockState2;
		//Next state
		stateHolder.nextState(std::make_unique<MockState>(mockState2));

		try {
			stateHolder.update();
			CHECK(false);
		} catch (ska::StateDiedException& sde) {
			Verify(Method(mockState, unload));
			Verify(Method(mockState2, load));

			auto lastEvent = stateListener.getLastStateEventTriggered();
			CHECK(lastEvent->type == ska::STATE_CHANGED);
		}
	}
}
