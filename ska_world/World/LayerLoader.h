#pragma once
#include "Utils/MovableNonCopyable.h"
#include "Utils/Vector2.h"

namespace ska {
	class Tileset;
    class Tile;
    class TileRenderable;

	class LayerLoader : public MovableNonCopyable {
	public :
		virtual Vector2<std::optional<ska::Tile>> loadPhysics(Tileset& chipset) const = 0;
		virtual Vector2<std::optional<const ska::TileRenderable>> loadGraphics(Tileset& chipset) const = 0;
		~LayerLoader() override = default;
	};

}
