#pragma once
#include <cstdint>

#include "Core/Point.h"
#include "TileCollision.h"
#include "TileProperties.h"

namespace ska {

	struct Tile {
		Tile() = default;
		Tile(Point<int> posChipset, TileProperties properties = {}, TileCollision col = TileCollision::No);
	
		// Its collision mask : MaskCells x MaskCells cells of the block, a bit by solid cell (bit row * MaskCells + column).
		// A tile blocking (collision Yes) with no mask in its tileset : all of it ; walked on : none
		static constexpr int MaskCells = 4;
		static constexpr std::uint16_t FullMask = 0xFFFF;

		Point<int> id {};
		TileCollision collision {};
		TileProperties properties {};
		std::uint16_t mask = FullMask;

		// Its solid cells : none when it is walked on
		std::uint16_t solidCells() const { return collision == TileCollision::Yes ? mask : 0; }
	};

}
