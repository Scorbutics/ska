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
		CHECK(lastEvent->type == ska::StateEventType::FIRST_STATE_LOAD);

		Verify(Method(mockState, loadBefore));
		Verify(Method(mockState, loadAfter));
	}

	SUBCASE("State change") {
		ska::StateHolder stateHolder(ged);
		StateListener stateListener(ged);

		Mock<ska::State> mockState;
		//First state
		stateHolder.nextState(std::make_unique<MockState>(mockState));
		stateHolder.update();

		Verify(Method(mockState, loadBefore));
		Verify(Method(mockState, loadAfter));

		Mock<ska::State> mockState2;
		//Next state
		stateHolder.nextState(std::make_unique<MockState>(mockState2));

		//try {
			stateHolder.update();
			//CHECK(false);
		//} catch (ska::StateDiedException& sde) {
			Verify(Method(mockState, unload));
			Verify(Method(mockState2, loadBefore));
			Verify(Method(mockState2, loadAfter));

			auto lastEvent = stateListener.getLastStateEventTriggered();
			CHECK(lastEvent->type == ska::StateEventType::STATE_CHANGE);
		//}
	}
}
