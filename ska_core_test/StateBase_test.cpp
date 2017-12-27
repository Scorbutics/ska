#include <doctest.h>
#include <fakeit.hpp>
#include "Draw/VectorDrawableContainer.h"
#include "Draw/Renderer.h"
#include "MockStateBase.h"
#include "Data/Events/GameEventDispatcher.h"
#include "Core/State/StateHolder.h"
#include "MockState.h"


TEST_CASE("[StateBase]") {
	using namespace fakeit;

	ska::GameEventDispatcher ged;
	//ska::EntityManager em(ged);

	MockStateBase stateBase;

	ska::StateHolder sh(ged);
	
	Mock<ska::State> mockState;
	Mock<ska::Renderer> renderer;

	ska::VectorDrawableContainer drawableContainer(renderer.get());

	SUBCASE("addSubState parent no active") {
		MockStateBase sbt;
		sbt.addSubState(std::make_unique<MockState>(mockState));
		
		SUBCASE("parent not loaded : substate is not loaded") {
			VerifyNoOtherInvocations(Method(mockState, load));
		}

		SUBCASE("loading parent state after adding the substate") {
			sbt.load(nullptr);

			SUBCASE("substate is loaded") {
				Verify(Method(mockState, load));
			}
		}
	}

	SUBCASE("addSubState parent active : substate automatically loaded") {
		MockStateBase sbt;
		
		//sets the state active
		sbt.load(nullptr);

		sbt.addSubState(std::make_unique<MockState>(mockState));

		Verify(Method(mockState, load));
	}


	SUBCASE("removeSubState"){
		MockStateBase sbt;
		auto sPtr = std::make_unique<MockState>(mockState);
		auto& state = *sPtr.get();
		sbt.addSubState(std::move(sPtr));

		SUBCASE("parent not loaded : substate is not unloaded") {
			sbt.removeSubState(state);
			VerifyNoOtherInvocations(Method(mockState, unload));
		}

		SUBCASE("loading parent after removing the substate") {
			sbt.load(nullptr);
			sbt.removeSubState(state);
			Verify(Method(mockState, unload));
		}
	}
}