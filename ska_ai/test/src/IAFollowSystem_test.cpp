#include <doctest.h>
#include <stdexcept>
#include "AI/__internalConfig/LoggerConfig.h"
#include "AI/System/IAFollowSystem.h"
#include "Core/ECS/EntityManager.h"
#include "Core/Data/Events/GameEventDispatcher.h"

namespace {
	struct FollowTest {
		ska::GameEventDispatcher ged;
		ska::EntityManager entities { ged };
		ska::IAFollowSystem system { entities };


		// The systems see the component changes once the entity manager is refreshed (as each frame of the game)
		void update() {
			entities.refresh();
			system.update(16);
			entities.refresh();
		}

		ska::EntityId character(float x, float y) {
			const auto entity = entities.createEntity();
			auto position = ska::PositionComponent{};
			position.x = static_cast<long>(x);
			position.y = static_cast<long>(y);
			entities.addComponent<ska::PositionComponent>(entity, std::move(position));
			auto hitbox = ska::HitboxComponent{};
			hitbox.xOffset = 0;
			hitbox.yOffset = 0;
			hitbox.width = 10;
			hitbox.height = 10;
			entities.addComponent<ska::HitboxComponent>(entity, std::move(hitbox));
			return entity;
		}
	};
}

TEST_CASE("[IAFollowSystem] an entity walks to the follow distance of its target") {
	auto test = FollowTest{};
	const auto target = test.character(300, 100);
	const auto follower = test.character(100, 100);
	test.entities.addComponent<ska::IAFollowComponent>(follower, ska::IAFollowComponent{ target, 50.F });
	test.update();

	REQUIRE(test.entities.hasComponent<ska::IADefinedMovementComponent>(follower));
	const auto& movement = test.entities.getComponent<ska::IADefinedMovementComponent>(follower);
	REQUIRE(movement.directions.size() == 1);
	// 200 pixels away, stops at 50 : walks 150 pixels to the right
	CHECK(movement.directions[0].x == 150);
	CHECK(movement.directions[0].y == 0);
	CHECK(movement.origin.x == 105);
	CHECK(movement.origin.y == 105);

	SUBCASE("while walking, no other step") {
		test.entities.getComponent<ska::IADefinedMovementComponent>(follower).directions[0].x = 1;
		test.update();
		CHECK(test.entities.getComponent<ska::IADefinedMovementComponent>(follower).directions[0].x == 1);
	}
}

TEST_CASE("[IAFollowSystem] close enough : the entity does not move") {
	auto test = FollowTest{};
	const auto target = test.character(100, 160);
	const auto follower = test.character(100, 100);
	test.entities.addComponent<ska::IAFollowComponent>(follower, ska::IAFollowComponent{ target, 48.F });
	test.update();
	CHECK(!test.entities.hasComponent<ska::IADefinedMovementComponent>(follower));
	CHECK(test.entities.hasComponent<ska::IAFollowComponent>(follower));
}

TEST_CASE("[IAFollowSystem] the following stops when the target is removed") {
	auto test = FollowTest{};
	const auto target = test.character(400, 100);
	const auto follower = test.character(100, 100);
	test.entities.addComponent<ska::IAFollowComponent>(follower, ska::IAFollowComponent{ target, 48.F });
	test.entities.removeEntity(target);
	test.update();
	CHECK(!test.entities.hasComponent<ska::IAFollowComponent>(follower));
	CHECK(!test.entities.hasComponent<ska::IADefinedMovementComponent>(follower));
}
