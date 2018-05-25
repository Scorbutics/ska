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

		auto cp = ska::CollisionProfile{ 1, std::move(layers) };
		
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
	}
}
