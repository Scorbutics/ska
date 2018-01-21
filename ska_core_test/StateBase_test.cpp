#include <doctest.h>
#include <fakeit.hpp>
#include "MockStateBase.h"
#include "Core/State/StateHolder.h"
#include "MockState.h"
#include "Task/Task.h"
#include "Draw/VectorDrawableContainer.h"
#include "../ska_gui_test/WindowsUtil.h"


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
		auto addedSubstate = &static_cast<MockState&>(sbt.addSubState(std::move(sPtr)));

		SUBCASE("bad substate removed"){
			Mock<ska::State> mSub2;
			sbt.scheduleRemoveSubState(mSub2());
			sbt.eventUpdate(0);
			VerifyNoOtherInvocations(Method(mSub2, unload));
		}

		SUBCASE("parent not loaded : substate is not unloaded") {
			sbt.scheduleRemoveSubState(state);
			sbt.eventUpdate(0);
			VerifyNoOtherInvocations(Method(mockState, unload));
		}

		SUBCASE("loading parent then removing the substate without updating, then refreshing") {
			sbt.load(nullptr);
			sbt.scheduleRemoveSubState(state);
			VerifyNoOtherInvocations(Method(mockState, unload));
			sbt.eventUpdate(0);
			Verify(Method(mockState, unload));
		}

		SUBCASE("loading parent then removing the substate (after load one)") {
			sbt.load(nullptr);
			sbt.scheduleRemoveSubState(*addedSubstate);
			sbt.eventUpdate(0);
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

		SUBCASE("loading with a substate") {
			sbt.addSubState(std::make_unique<MockState>(mockState));

			sbt.load(nullptr);
			Verify(Method(mockState, load));
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

	SUBCASE("graphicUpdate") {
		MockStateBase sbt;
		MockRenderer mr;
		ska::VectorDrawableContainer vdc(mr);
		sbt.load(nullptr);
		
		SUBCASE("no substate") {
			SUBCASE("basic") {
				sbt.graphicUpdate(0, vdc);

				CHECK(sbt.read.onGraphicUpdate_);
			}

			SUBCASE("with graphic system") {
				auto& gs = sbt.addMockGraphicSystem();
				sbt.graphicUpdate(0, vdc);

				CHECK(gs.read.drawablesDone_);
				CHECK(gs.read.drawables_ == &vdc);
				CHECK(gs.read.update_);
			}
		}

		SUBCASE("with substate") {
			auto sPtr = std::make_unique<MockState>(mockState);
			sbt.addSubState(std::move(sPtr));

			sbt.graphicUpdate(0, vdc);

			Verify(Method(mockState, graphicUpdate));
		}

		SUBCASE("with linked substate") {
			MockState state { mockState };
			sbt.linkSubState(state);

			sbt.graphicUpdate(0, vdc);

			Verify(Method(mockState, graphicUpdate));
		}
	}

	SUBCASE("eventUpdate") {
		MockStateBase sbt;
		MockRenderer mr;
		sbt.load(nullptr);

		SUBCASE("no substate") {
			SUBCASE("basic") {
				sbt.eventUpdate(0);

				CHECK(sbt.read.onEventUpdate_);
			}

			SUBCASE("with system") {
				auto& s = sbt.addMockSystem();
				sbt.eventUpdate(0);
				
				CHECK(s.read.update_);
			}
		}

		SUBCASE("with substate") {
			auto sPtr = std::make_unique<MockState>(mockState);
			sbt.addSubState(std::move(sPtr));

			sbt.eventUpdate(0);

			Verify(Method(mockState, eventUpdate));
		}

		SUBCASE("with linked substate") {
			MockState state{ mockState };
			sbt.linkSubState(state);

			sbt.eventUpdate(0);

			Verify(Method(mockState, eventUpdate));
		}
	}

	SUBCASE("adding systems") {
		MockStateBase sbt;
		MockRenderer mr;
		sbt.load(nullptr);

		SUBCASE("logic priority") {
			SUBCASE("asc") {
				auto& m1 = sbt.addPriorizedMockSystem(14);
				auto& m2 = sbt.addPriorizedMockSystem(21);

				int order[2] = {};
				auto index = 0;
				m1.whenUpdate([&order, &index]() {
					order[index++] = 1;
				});

				m2.whenUpdate([&order, &index]() {
					order[index++] = 2;
				});

				sbt.eventUpdate(0);
				CHECK(order[0] == 1);
				CHECK(order[1] == 2);
			}

			SUBCASE("desc"){
				auto& m1 = sbt.addPriorizedMockSystem(38);
				auto& m2 = sbt.addPriorizedMockSystem(12);

				int order[2] = {};
				auto index = 0;
				m1.whenUpdate([&order, &index]() {
					order[index++] = 1;
				});

				m2.whenUpdate([&order, &index]() {
					order[index++] = 2;
				});

				sbt.eventUpdate(0);
				CHECK(order[0] == 2);
				CHECK(order[1] == 1);
			}

			SUBCASE("mixed with default priorization") {
				auto& m1 = sbt.addPriorizedMockSystem(38);
				auto& m2 = sbt.addPriorizedMockSystem(12);
				auto& m3 = sbt.addMockSystem();

				int order[3] = {};
				auto index = 0;
				m1.whenUpdate([&order, &index]() {
					order[index++] = 1;
				});

				m2.whenUpdate([&order, &index]() {
					order[index++] = 2;
				});

				m3.whenUpdate([&order, &index]() {
					order[index++] = 3;
				});

				sbt.eventUpdate(0);
				CHECK(order[0] == 3);
				CHECK(order[1] == 2);
				CHECK(order[2] == 1);
			}
		}

		SUBCASE("graphic priority") {
			MockRenderer mr;
			ska::VectorDrawableContainer vdc(mr);

			SUBCASE("asc") {
				auto& m1 = sbt.addPriorizedMockGraphicSystem(14);
				auto& m2 = sbt.addPriorizedMockGraphicSystem(21);

				int order[2] = {};
				auto index = 0;
				m1.whenUpdate([&order, &index]() {
					order[index++] = 1;
				});

				m2.whenUpdate([&order, &index]() {
					order[index++] = 2;
				});

				sbt.graphicUpdate(0, vdc);
				CHECK(order[0] == 1);
				CHECK(order[1] == 2);
			}

			SUBCASE("desc") {
				auto& m1 = sbt.addPriorizedMockGraphicSystem(38);
				auto& m2 = sbt.addPriorizedMockGraphicSystem(12);

				int order[2] = {};
				auto index = 0;
				m1.whenUpdate([&order, &index]() {
					order[index++] = 1;
				});

				m2.whenUpdate([&order, &index]() {
					order[index++] = 2;
				});

				sbt.graphicUpdate(0, vdc);
				CHECK(order[0] == 2);
				CHECK(order[1] == 1);
			}

			SUBCASE("mixed with default priorization") {
				auto& m1 = sbt.addPriorizedMockGraphicSystem(38);
				auto& m2 = sbt.addPriorizedMockGraphicSystem(12);
				auto& m3 = sbt.addMockGraphicSystem();

				int order[3] = {};
				auto index = 0;
				m1.whenUpdate([&order, &index]() {
					order[index++] = 1;
				});

				m2.whenUpdate([&order, &index]() {
					order[index++] = 2;
				});

				m3.whenUpdate([&order, &index]() {
					order[index++] = 3;
				});

				sbt.graphicUpdate(0, vdc);
				CHECK(order[0] == 3);
				CHECK(order[1] == 2);
				CHECK(order[2] == 1);
			}
		}
	}
}