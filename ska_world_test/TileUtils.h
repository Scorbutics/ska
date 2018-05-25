#pragma once
#include <optional>
#include "Utils/Vector2.h"
#include "World/Tile.h"
#include "World/CollisionProfile.h"

namespace TileWorldPhysics {
	ska::Vector2<std::optional<ska::Tile>> BuildTiles(const ska::Vector2<char>& collisions, const ska::Vector2<int>& properties);
	ska::CollisionProfile BuildCollisionProfile(const ska::Vector2<char>& collisions, const ska::Vector2<int>& properties = ska::Vector2<int>{});
}
