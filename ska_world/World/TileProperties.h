#pragma once
namespace ska {
	enum TilePropertiesType {
		TILE_PROPERTY_WATER = 1
	};

	struct TileProperties {
		unsigned int bitMask {};
	};
}