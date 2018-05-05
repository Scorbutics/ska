#include <optional>
#include <unordered_set>
#include "TileAgglomerate.h"
#include "CollisionProfile.h"

struct MergedTile {
	std::optional<ska::Rectangle> area;
	MergedTile* merged = nullptr;

	MergedTile& operator=(const MergedTile& mergedTile) = default;
};

bool TileAgglomerateCheckSizeNotMerged(unsigned int blockSize, const std::optional<ska::Rectangle>& value, bool horizontal) {
	if (!value.has_value()) {
		return false;
	}
	return horizontal ? value->h == blockSize : value->w == blockSize;
}

MergedTile& TileAgglomerateMergeTile(MergedTile& currentTileToMerge, ska::Rectangle currentArea, unsigned int blockSize, MergedTile* withLastTile, bool horizontal) {
	const auto canBeMerged = withLastTile != nullptr && (TileAgglomerateCheckSizeNotMerged(blockSize, withLastTile->area, horizontal));
	if (!canBeMerged) {
		currentTileToMerge.area = currentArea;
	}
	else {
		auto& originTile = withLastTile->merged;

		if (originTile == nullptr) {
			originTile = withLastTile;
		}

		if (horizontal) {
			originTile->area->w += blockSize;
		}
		else {
			originTile->area->h += blockSize;
		}

		currentTileToMerge.merged = originTile;
		currentTileToMerge.area = std::optional<ska::Rectangle>();
		return *originTile;
	}
	return currentTileToMerge;
}

//TODO rendre le code plus lisible et explicite
void TileAgglomerate(const ska::CollisionProfile& world, ska::Vector2<MergedTile>& layer, bool horizontal) {
	const int blockSize = world.getBlockSize();
	const auto width = world.getBlocksX();
	const auto height = world.getBlocksY();
	const int area = width * height;

	const auto dimension = horizontal ? width : height;

	MergedTile* lastTile = nullptr;

	for (auto i = 0; i < area; i++) {
		if(i % dimension == 0) {
			lastTile = nullptr;
		}

		const auto x = horizontal ? i % width : i / height;
		const auto y = horizontal ? i / width : i % height;
		const auto col = world.collide(x, y);
		
		const auto isMerged = layer[x][y].merged != nullptr;
		if (col && !isMerged) {
			const auto currentArea = ska::Rectangle{ static_cast<int>(x * blockSize), static_cast<int>(y * blockSize), blockSize, blockSize };
			lastTile = &TileAgglomerateMergeTile(layer[x][y], currentArea, blockSize, lastTile, horizontal);
		} else {
			lastTile = &layer[x][y];
		}

	}
	
}

std::vector<ska::Rectangle> ska::TileAgglomerate(const ska::CollisionProfile& world, ska::TileAgglomerationPriority priority) {
	ska::Vector2<MergedTile> layer;
	const auto width = world.getBlocksX();
	const auto height = world.getBlocksY();
	layer.resize(width, height);

	TileAgglomerate(world, layer, priority == ska::TileAgglomerationPriority::HORIZONTAL);
	TileAgglomerate(world, layer, priority == ska::TileAgglomerationPriority::VERTICAL);

	std::unordered_set<Rectangle> set;
	for(const auto& b : layer) {
		if(b.area.has_value()) {
			set.emplace(b.area.value());
		}
	}

	std::vector<ska::Rectangle> result;
	result.resize(set.size());
	std::copy(set.cbegin(), set.cend(), result.begin());
	return result;
}

