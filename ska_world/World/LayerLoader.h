#pragma once

namespace ska {
	class Tileset;

	class LayerLoader : public MovableNonCopyable {
	public :
		virtual Vector2<Tile*> loadPhysics(Tileset& chipset) const = 0;
		virtual Vector2<const TileRenderable*> loadGraphics(Tileset& chipset) const = 0;
		~LayerLoader() override = default;
	};

}
