#pragma once
#include <vector>
#include <optional>
#include "Rectangle.h"
#include "Utils/Vector2.h"

namespace ska {
	class TileWorld;

	enum class TileAgglomerationPriority {
		HORIZONTAL,
		VERTICAL
	};

	class TileAgglomerate {
	public: 
		TileAgglomerate() = delete;
		~TileAgglomerate() = default;

		static std::vector<ska::Rectangle> apply(const ska::TileWorld& world, TileAgglomerationPriority priority = TileAgglomerationPriority::HORIZONTAL);
		
	private:
		struct MergedTile {
			std::optional<ska::Rectangle> area;
			MergedTile* merged = nullptr;

			MergedTile& operator=(const MergedTile& mergedTile) = default;
		};

		static bool checkSize(unsigned int blockSize, const std::optional<ska::Rectangle>& value, bool horizontal);
		static MergedTile& mergeTile(MergedTile& currentTileToMerge, ska::Rectangle currentArea, unsigned int blockSize, MergedTile* withLastTile, bool horizontal);
		static void apply(const ska::TileWorld& world, Vector2<MergedTile>& layer, bool horizontal);
	};
}
