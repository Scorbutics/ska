#include <doctest.h>
#include "Core/__internalConfig/LoggerConfig.h"
#include "Core/ECS/ComponentHandler.h"

TEST_CASE("[ComponentHandler]") {
	constexpr auto mask = 1u;
	auto componentHandlerTest = ska::ComponentHandler<int> { mask };
	
	SUBCASE("Add") {
		auto entityId = ska::EntityId{1};
		CHECK(componentHandlerTest.add(entityId, 14) == mask);
		CHECK(componentHandlerTest.add(entityId, 128) == mask);
	}

	SUBCASE("AddEmpty") {
		auto entityId = ska::EntityId { 1 };
		componentHandlerTest.add(entityId, 14);
		CHECK(componentHandlerTest.addEmpty(entityId) == mask);
		CHECK(componentHandlerTest.getComponent(entityId) == 14);
	}

	SUBCASE("Get") {
		auto entityId = ska::EntityId { 3 };
		componentHandlerTest.add(entityId, 14);
		CHECK(componentHandlerTest.getComponent(entityId) == 14);
	}

	SUBCASE("Remove") {
		auto entityId = ska::EntityId { 7 };
		componentHandlerTest.add(entityId, 14);
		CHECK(componentHandlerTest.remove(entityId) == mask);
		componentHandlerTest.add(entityId, 18);
		CHECK(componentHandlerTest.getComponent(entityId) == 18);
	}
	
	SUBCASE("Add 2 entities") {
		auto entityId = ska::EntityId { 7 };
		auto entityId2 = ska::EntityId { 17 };
		componentHandlerTest.add(entityId, 14);
		componentHandlerTest.add(entityId2, 47);
		CHECK(componentHandlerTest.getComponent(entityId) == 14);
		CHECK(componentHandlerTest.getComponent(entityId2) == 47);

		SUBCASE("then remove entity 1") {
			componentHandlerTest.remove(entityId);
			CHECK(componentHandlerTest.getComponent(entityId2) == 47);

			SUBCASE("then add entity with same entity 1 id") {
				auto reentity = ska::EntityId { 7 };
				componentHandlerTest.add(reentity, 1165);
				CHECK(componentHandlerTest.getComponent(entityId) == 1165);
			}
		}
	}

}