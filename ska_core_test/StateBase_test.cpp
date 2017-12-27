#include <doctest.h>
#include <fakeit.hpp>
#include "MockStateBase.h"
#include "Core/State/StateHolder.h"
#include "MockState.h"
#include "Task/Task.h"


TEST_CASE("[StateBase]") {
	using namespace fakeit;
	
	Mock<ska::State> mockState;

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


	SUBCASE("removeSubState") {
		MockStateBase sbt;
		auto sPtr = std::make_unique<MockState>(mockState);
		auto& state = *sPtr.get();
		sbt.addSubState(std::move(sPtr));

		SUBCASE("bad substate removed"){
			Mock<ska::State> mSub2;
			sbt.removeSubState(mSub2());
			VerifyNoOtherInvocations(Method(mSub2, unload));
		}

		SUBCASE("parent not loaded : substate is not unloaded") {
			sbt.removeSubState(state);
			VerifyNoOtherInvocations(Method(mockState, unload));
		}

		SUBCASE("loading parent then removing the substate") {
			sbt.load(nullptr);
			sbt.removeSubState(state);
			Verify(Method(mockState, unload));
		}
	}

	SUBCASE("loading") {
		MockStateBase sbt;

		SUBCASE("basic (direct loading)") {
			sbt.load(nullptr);
			CHECK(sbt.read.beforeLoadStateDone_);
			CHECK(sbt.read.afterLoadStateDone_);
		}
	}

	SUBCASE("unloading") {
		MockStateBase sbt;
		sbt.load(nullptr);

		SUBCASE("basic (direct unloading)") {
			sbt.unload();
			CHECK(sbt.read.beforeUnload_);
			CHECK(sbt.read.afterUnload_);
		}

		SUBCASE("with one task with only one iteration") {
			sbt.task(std::make_unique<ska::RunnableLambda>([]() {
				return false;
			}));
			sbt.unload();

			CHECK(sbt.read.beforeUnload_);
			CHECK(sbt.read.afterUnload_);
		}

		SUBCASE("with one task with 2 iterations") {
			auto counterTask = 0;
			sbt.task(std::make_unique<ska::RunnableLambda>([&counterTask]() {
				return counterTask++ == 0;
			}));
			sbt.unload();

			CHECK(sbt.read.beforeUnload_);
			CHECK(!sbt.read.afterUnload_);
			
			sbt.unload();

			CHECK(sbt.read.afterUnload_);
		}


		SUBCASE("with at least one substate") {
			auto sPtr = std::make_unique<MockState>(mockState);
			auto& state = *sPtr.get();
			sbt.addSubState(std::move(sPtr));

			SUBCASE("basic (direct unloading)") {
				sbt.unload();

				CHECK(sbt.read.beforeUnload_);
				CHECK(sbt.read.afterUnload_);

				Verify(Method(mockState, unload));
			}

			SUBCASE("substate with one task with 2 iterations") {
				//Tests that the first substate unloading iteration doesn't fully load the main state, but the second does
				Method(mockState, unload) = true;
				sbt.unload();

				CHECK(sbt.read.beforeUnload_);
				CHECK(!sbt.read.afterUnload_);

				Method(mockState, unload) = false;
				sbt.unload();

				CHECK(sbt.read.afterUnload_);
			}

			SUBCASE("3 substate successively unloaded with iterations") {
				//Tests the fact that ALL substates must be loaded in order to fully unload the main state.
				Mock<ska::State> mockState2;
				sbt.addSubState(std::make_unique<MockState>(mockState2));
				Mock<ska::State> mockState3;
				sbt.addSubState(std::make_unique<MockState>(mockState3));

				Method(mockState3, unload) = true;
				Method(mockState2, unload) = true;
				Method(mockState, unload) = true;
				sbt.unload();

				CHECK(sbt.read.beforeUnload_);
				CHECK(!sbt.read.afterUnload_);

				Method(mockState3, unload) = false;
				Method(mockState2, unload) = false;
				sbt.unload();

				CHECK(!sbt.read.afterUnload_);

				Method(mockState, unload) = false;
				sbt.unload();

				CHECK(sbt.read.afterUnload_);
			}
		}
	}
}