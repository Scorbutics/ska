#include <doctest.h>
#include <optional>
#include "World/Layer.h"
#include "World/Tile.h"

ska::Vector2<std::optional<ska::Tile>> BuildTiles() {
    static constexpr auto width = 10;
    static constexpr auto height = 10;

    auto tiles = ska::Vector2<std::optional<ska::Tile>> {};
    tiles.resize(width, height);

    tiles[1][7] = {{1, 7}, ska::TileProperties {}, ska::TileCollision::Yes};
    return tiles;
}

TEST_CASE("[Layer]"){
	const auto tiles = BuildTiles();
	auto layer = ska::Layer{ tiles };

	SUBCASE("Size") {
        CHECK(layer.getBlocksX() == 10);
        CHECK(layer.getBlocksY() == 10);
	}

	SUBCASE("Collision") {
		CHECK(layer.getCollision(1, 7) == ska::TileCollision::Yes);
		CHECK(layer.getCollision(4, 9) == ska::TileCollision::Void);
	}

	SUBCASE("Collision out of bounds") {
		CHECK(layer.getCollision(10000, 7041) == ska::TileCollision::Yes);
	}

	SUBCASE("Block") {
		CHECK(layer.getBlock(1, 7) != nullptr);
		CHECK(layer.getBlock(4, 9) == nullptr);
	}

	SUBCASE("Block out of bounds") {
		CHECK(layer.getBlock(1000, 7000) == nullptr);
	}

}
