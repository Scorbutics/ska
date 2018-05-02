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

    auto tiles = BuildTiles();
	SUBCASE("Loading") {
        auto layer = ska::Layer { tiles };
        /*
        CHECK(layer.getBlocksX() == 10);
        CHECK(layer.getBlocksY() == 10);

		CHECK(layer.getCollision(1, 7) == ska::TileCollision::Yes);
        CHECK(layer.getBlock(1, 7) != nullptr);

        CHECK(layer.getCollision(4, 9) == ska::TileCollision::Void);
        CHECK(layer.getBlock(4, 9) == nullptr);
        */
	}
}
