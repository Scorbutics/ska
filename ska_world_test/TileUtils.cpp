#include "TileUtils.h"

ska::Vector2<std::optional<ska::Tile>> TileWorldPhysics::BuildTiles(const ska::Vector2<char>& collisions, const ska::Vector2<int>& properties) {
	const auto width = collisions.lineSize();
	const auto height = collisions.size() / collisions.lineSize();

	auto tiles = ska::Vector2<std::optional<ska::Tile>>{};
	tiles.resize(width, height);

	for (auto x = 0; x < width; x++) {
		for (auto y = 0; y < height; y++) {
			auto tileProperties = ska::TileProperties{};
			tileProperties.bitMask = !properties.has(x, y) ? 0 : properties[x][y];
			tiles[x][y] = ska::Tile{ { x, y }, tileProperties,
				collisions[x][y] == 0 ? ska::TileCollision::No :
				(collisions[x][y] == '-' ? ska::TileCollision::Void : ska::TileCollision::Yes)
			};
		}
	}

	return tiles;
}

ska::CollisionProfile TileWorldPhysics::BuildCollisionProfile(const ska::Vector2<char>& collisions, const ska::Vector2<int>& properties) {
	const auto tiles = TileWorldPhysics::BuildTiles(collisions, properties);
	auto layers = std::vector<ska::LayerPtr>{};
	layers.emplace_back(std::make_unique<ska::Layer>(tiles));
	return ska::CollisionProfile{ 1, std::move(layers) };
}
