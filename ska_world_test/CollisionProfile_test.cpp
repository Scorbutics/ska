#include <doctest.h>
#include "World/CollisionProfile.h"
#include "TileUtils.h"

TEST_CASE("[CollisionProfile]"){

	SUBCASE("4x4 2 layers complex") {
		constexpr auto width = 4u;
		const auto& collisions1 = ska::Vector2<char>{ width, {
			0, 0, 0, 0,
			0, 1, 0, 0,
			0, 1, 1, 0,
			0, 0, 1, 0
		} };
		const auto& properties1 = ska::Vector2<int>{};

		const auto& collisions2 = ska::Vector2<char>{ width,{
			0, 1, 1, 0,
			0, 1, 1, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		} };

		const auto& properties2 = ska::Vector2<int>{ width, {
			1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, 1, 1
		} };

		auto layer1 = std::make_unique<ska::Layer>(TileWorldPhysics::BuildTiles(collisions1, properties1));
		auto layer2 = std::make_unique<ska::Layer>(TileWorldPhysics::BuildTiles(collisions2, properties2));

		auto layers = std::vector<ska::LayerPtr>{};
		layers.push_back(std::move(layer1));
		layers.push_back(std::move(layer2));

		auto cp = ska::CollisionProfile{ 10, std::move(layers) };

		SUBCASE("Block size, dimensions") {
		    CHECK(cp.getBlockSize() == 1);
		    CHECK(cp.getBlocksX() == 4);
		    CHECK(cp.getBlocksY() == 4);
		}

		SUBCASE("Layers") {
		    CHECK(!cp.empty());
		    CHECK(cp.layers() == 2);
		}

		SUBCASE("getHighestCollidingBlock layer top max 1") {
			SUBCASE("0 0") {
				CHECK(cp.getHighestCollidingBlock(1, 0, 0) == nullptr);
			}

			SUBCASE("1 0") {
				auto tile = cp.getHighestCollidingBlock(1, 1, 0);
				CHECK(tile != nullptr);
				CHECK(tile->id == ska::Point<int>{1, 0});
				CHECK(tile->properties.bitMask == 1);
			}

			SUBCASE("1 2") {
				auto tile = cp.getHighestCollidingBlock(1, 1, 2);
				CHECK(tile != nullptr);
				CHECK(tile->id == ska::Point<int>{1, 2});
				CHECK(tile->properties.bitMask == 0);
			}
		}

        SUBCASE("getHighestCollidingBlock layer top max 0") {
			SUBCASE("0 0") {
				CHECK(cp.getHighestCollidingBlock(0, 0, 0) == nullptr);
			}

			SUBCASE("1 0") {
				auto tile = cp.getHighestCollidingBlock(0, 1, 0);
				CHECK(tile == nullptr);
			}

			SUBCASE("1 2") {
				auto tile = cp.getHighestCollidingBlock(0, 1, 2);
				CHECK(tile != nullptr);
				CHECK(tile->id == ska::Point<int>{1, 2});
				CHECK(tile->properties.bitMask == 0);
			}
		}

        SUBCASE("collide layer top max 0") {
			SUBCASE("0 0") {
				CHECK(!cp.collide(0, 0, 0));
			}

			SUBCASE("1 0") {
				CHECK(!cp.collide(0, 1, 0));
			}

			SUBCASE("1 2") {
				CHECK(cp.collide(0, 1, 2));
			}
		}

		SUBCASE("collide layer top max 1") {
			SUBCASE("0 0") {
				CHECK(!cp.collide(1, 0, 0));
			}

			SUBCASE("1 0") {
				CHECK(cp.collide(1, 1, 0));
			}

			SUBCASE("1 2") {
				CHECK(cp.collide(1, 1, 2));
			}
		}

        SUBCASE("getBlock layer 1") {
			SUBCASE("0 0") {
				CHECK(cp.getBlock(1, 0, 0) == nullptr);
			}

			SUBCASE("1 0") {
				CHECK(cp.getBlock(1, 1, 0) != nullptr);
			}

			SUBCASE("1 2") {
				CHECK(cp.getBlock(1, 1, 2) == nullptr);
			}
		}

		SUBCASE("getBlock layer 0") {
			SUBCASE("0 0") {
				CHECK(cp.getBlock(0, 0, 0) == nullptr);
			}

			SUBCASE("1 0") {
				CHECK(cp.getBlock(0, 1, 0) == nullptr);
			}

			SUBCASE("1 2") {
				CHECK(cp.getBlock(0, 1, 2) != nullptr);
			}
		}

		SUBCASE("alignOnBlock") {
		    SUBCASE("nominal") {
                const auto hitbox = ska::Rectangle { 14, 22, 23, 11};
                const auto pointAligned = cp.alignOnBlock(hitbox);
                CHECK(pointAligned.x == 4);
                CHECK(pointAligned.y == 2);
		    }

		    SUBCASE("borne sup") {
                const auto hitbox = ska::Rectangle { 19, 29, 1, 1};
                const auto pointAligned = cp.alignOnBlock(hitbox);
                CHECK(pointAligned.x == 9);
                CHECK(pointAligned.y == 9);
		    }

		    SUBCASE("borne inf") {
                const auto hitbox = ska::Rectangle { 10, 20, 1, 1};
                const auto pointAligned = cp.alignOnBlock(hitbox);
                CHECK(pointAligned.x == 0);
                CHECK(pointAligned.y == 0);
		    }
		}

		//TODO placeOnNearestPracticableBlock
	}
}
