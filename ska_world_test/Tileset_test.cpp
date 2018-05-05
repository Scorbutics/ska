#include <doctest.h>
#include <fakeit.hpp>
#include "World/Tileset.h"

TEST_CASE("[Tileset]"){

    auto loader = fakeit::Mock<ska::TilesetLoader>{};
	auto eventLoader = fakeit::Mock<ska::TilesetEventLoader>{};

	Method(loader, loadGraphics) = ska::Texture{};
	Method(loader, getName) = "Toto Tileset";

	auto animations = ska::Vector2<ska::TileAnimation>{};
	animations.resize(5, 5);
	Method(loader, loadAnimations) = animations;

	auto tiles = ska::Vector2<ska::Tile>{};
	tiles.resize(5, 5);
	tiles[1][4] = ska::Tile{ {1, 4}, ska::TileProperties{}, ska::TileCollision::Yes };
	Method(loader, loadPhysics) = tiles;
	
	auto ssc = ska::ScriptSleepComponent{};
	ssc.context = "test context";
	ssc.deleteEntityWhenFinished = true;
	ssc.id = 1234;
	ssc.name = "test name";
	ssc.period = 1000;
	ssc.triggeringType = ska::ScriptTriggerType::TOUCH;

	auto scripts = std::vector<ska::ScriptSleepComponent>{ ssc };
	Method(eventLoader, load) = scripts;

    auto tileset = ska::Tileset {48, loader.get(), eventLoader.get()};

    SUBCASE("Tile size") {
        CHECK(tileset.getTileSize() == 48);
    }
}
