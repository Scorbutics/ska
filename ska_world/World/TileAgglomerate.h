#pragma once
#include <vector>
#include "Rectangle.h"
#include <optional>
#include "Utils/Vector2.h"

namespace ska {
	class TileWorld;

	struct MergedTile {
		std::optional<ska::Rectangle> area;
		MergedTile* merged = nullptr;

		ska::MergedTile& operator=(const MergedTile& mergedTile) = default;
	};

	class TileAgglomerate {
	public: 
		TileAgglomerate() = delete;
		~TileAgglomerate() = default;

		static std::vector<ska::Rectangle> apply(const ska::TileWorld& world);
		
	private:
		static bool checkSize(unsigned int blockSize, const std::optional<ska::Rectangle>& value, bool horizontal);
		static ska::MergedTile& mergeTile(MergedTile& currentTileToMerge, ska::Rectangle currentArea, unsigned blockSize,
		                           MergedTile* withLastTile, bool horizontal);
		static void apply(const ska::TileWorld& world, Vector2<MergedTile>& layer, bool horizontal);
	};
}
