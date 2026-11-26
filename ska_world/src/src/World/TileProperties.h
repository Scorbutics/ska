#pragma once

namespace ska {
	enum TilePropertiesType {
		TILE_PROPERTY_WATER = 1
	};

	struct TileProperties {
		unsigned int bitMask {};
		// Where the object the tile is a part of stands on the ground, in blocks under the tile : -1, the tile is flat
		// (a ground, a carpet : drawn under the characters) ; 0, it stands on its own block (a sign, a trunk) ; n, the
		// tile is the upper part of an object standing n blocks lower (the lantern of a lamp : 1, the roof of a house of
		// 5 rows : 4). The layers draw a standing tile sorted with the characters by that base line (LayerDepthRenderable).
		// From the properties image of the tileset (TilesetLoaderImage : its green channel, 0 flat, n + 1 a base n).
		int base = -1;

		bool standing() const { return base >= 0; }
	};
}
