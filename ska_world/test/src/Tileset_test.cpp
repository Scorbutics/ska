#include <doctest.h>
#include <fakeit.hpp>
#include "World/Tileset.h"

TEST_CASE("[Tileset]"){

    auto loader = fakeit::Mock<ska::TilesetLoader>{};
	auto eventLoader = fakeit::Mock<ska::TilesetEventLoader>{};

	auto texture = ska::Texture{};
	texture.loadFromColoredRect(12, 14, ska::Color{14, 11, 23, 255});
	Method(loader, loadGraphics) = texture;
	Method(loader, getName) = "Toto Tileset";

	auto animations = ska::Vector2<ska::TileAnimation>{};
	animations.resize(5, 5);
	animations[4][2].animated = true;
	animations[4][2].animation = ska::Animation{ 300, 2, false, 0, 0, 4, 4 };

	Method(loader, loadAnimations) = animations;

	auto tiles = ska::Vector2<ska::Tile>{};
	tiles.resize(5, 5);
	tiles[1][4] = ska::Tile{ {1, 4}, ska::TileProperties{}, ska::TileCollision::Yes };
	Method(loader, loadPhysics) = tiles;

	const auto ssc = ska::ScriptSleepComponent{};
	const auto scripts = std::vector<ska::ScriptSleepComponent>{ ssc };
	Method(eventLoader, load) = scripts;

    auto tileset = ska::Tileset {48, loader.get(), eventLoader.get()};

    SUBCASE("Tile size") {
        CHECK(tileset.getTileSize() == 48);
    }

	SUBCASE("Tile Animation") {
		CHECK(!tileset.getAnimation({ 0, 0 }).animated);
		const auto check = tileset.getAnimation({ 4, 2 }).animation.getOffsetBase() == ska::Rectangle{ 0, 0, 4, 4 };
		CHECK(check);
	}

	SUBCASE("Tileset name") {
		CHECK(tileset.getName() == "Toto Tileset");
	}

	SUBCASE("Tileset dimensions") {
		CHECK(tileset.getWidth() == 12);
		CHECK(tileset.getHeight() == 14);
	}

	SUBCASE("Tileset texture") {
		CHECK(tileset.getTexture().getWidth() == tileset.getWidth());
		CHECK(tileset.getTexture().getHeight() == tileset.getHeight());
	}

}
