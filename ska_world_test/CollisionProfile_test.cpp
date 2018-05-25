#include <doctest.h>
#include "World/CollisionProfile.h"

TEST_CASE("[CollisionProfile]"){
	auto tiles = BuildTiles();
	auto layer = std::make_unique<ska::Layer>(std::move(tiles));
	auto layers = std::vector<ska::LayerPtr>{};
	layers.push_back(std::move(layer));
    auto cp = ska::CollisionProfile{1, std::move(layers)};

	SUBCASE("Init") {

	}
}
