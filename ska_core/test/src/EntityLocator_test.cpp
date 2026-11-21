#include <doctest.h>
#include "LoggerConfig.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/EntityLocator.h"

TEST_CASE("[EntityLocator] finds the named entities, until they are removed") {
	auto ged = ska::GameEventDispatcher {};
	auto em = ska::EntityManager{ ged };
	auto locator = ska::EntityLocator{ ged };

	const auto trainer = em.createEntity();
	const auto other = em.createEntity();
	CHECK(locator.getEntityId("trainer") == nullptr);

	em.setName(trainer, "trainer");
	em.setName(other, "npc");
	REQUIRE(locator.getEntityId("trainer") != nullptr);
	CHECK(*locator.getEntityId("trainer") == trainer);
	CHECK(*locator.getEntityId("npc") == other);

	SUBCASE("renamed") {
		em.setName(other, "robert");
		CHECK(locator.getEntityId("npc") == nullptr);
		CHECK(*locator.getEntityId("robert") == other);
	}

	SUBCASE("removed") {
		em.removeEntity(other);
		CHECK(locator.getEntityId("npc") == nullptr);
		CHECK(*locator.getEntityId("trainer") == trainer);
	}
}
