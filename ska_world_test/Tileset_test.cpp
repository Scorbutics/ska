#include <doctest.h>
#include <fakeit.hpp>
#include "World/Tileset.h"

TEST_CASE("[Tileset]"){

    auto loader = fakeit::Mock<ska::TilesetLoader>{};
    auto eventLoader = fakeit::Mock<ska::TilesetEventLoader>{};

    auto tileset = ska::Tileset {48, loader.get(), eventLoader.get()};

    SUBCASE("Tile size") {
        CHECK(tileset.getTileSize() == 48);
    }
}
