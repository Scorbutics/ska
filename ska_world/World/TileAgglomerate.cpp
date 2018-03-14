#include "TileAgglomerate.h"
#include "TileWorld.h"

//TODO rendre le code plus lisible et explicite
void ska::TileAgglomerate::apply(const ska::TileWorld& world, ska::Vector2<MergedTile>& layer, bool horizontal) {
	const int blockSize = world.getBlockSize();
	const auto width = world.getNbrBlocX();
	const auto height = world.getNbrBlocY();
	const int area = width * height;

	const auto dimension = horizontal ? width : height;

	MergedTile* lastTile = nullptr;

	for (auto i = 0; i < area; i++) {
		if(i % dimension == 0) {
			lastTile = nullptr;
		}

		const auto x = horizontal ? i % width : i / height;
		const auto y = horizontal ? i / width : i % height;
		const auto col = world.getCollision(x, y);
		
		const auto isMerged = layer[x][y].merged != nullptr;
		if (col && !isMerged) {
			const auto currentArea = ska::Rectangle{ static_cast<int>(x * blockSize), static_cast<int>(y * blockSize), blockSize, blockSize };
			lastTile = &mergeTile(layer[x][y], currentArea, blockSize, lastTile, horizontal);
		} else {
			lastTile = &layer[x][y];
		}

	}
	
}

std::vector<ska::Rectangle> ska::TileAgglomerate::apply(const ska::TileWorld& world, TileAgglomerationPriority priority) {
	ska::Vector2<MergedTile> layer;
	const auto width = world.getNbrBlocX();
	const auto height = world.getNbrBlocY();
	layer.resize(width, height);

	apply(world, layer, priority == TileAgglomerationPriority::HORIZONTAL);
	apply(world, layer, priority == TileAgglomerationPriority::VERTICAL);

	std::unordered_set<ska::Rectangle> set;
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

bool ska::TileAgglomerate::checkSize(unsigned int blockSize, const std::optional<ska::Rectangle>& value, bool horizontal) {
	if(!value.has_value()) {
		return false;
	}
	return horizontal ? value->h == blockSize : value->w == blockSize;
}

ska::TileAgglomerate::MergedTile& ska::TileAgglomerate::mergeTile(MergedTile& currentTileToMerge, ska::Rectangle currentArea, unsigned int blockSize, MergedTile* withLastTile, bool horizontal) {
	const auto canBeMerged = withLastTile != nullptr && (checkSize(blockSize, withLastTile->area, horizontal));	
	if (!canBeMerged) {
		currentTileToMerge.area = currentArea;
	} else {
		auto& originTile = withLastTile->merged;

		if(originTile == nullptr) {
			originTile = withLastTile;
		}
		
		if (horizontal)  {
			originTile->area->w += blockSize;										
		} else {
			originTile->area->h += blockSize;
		}

		currentTileToMerge.merged = originTile;
		currentTileToMerge.area = std::optional<ska::Rectangle>();
		return *originTile;
	}
	return currentTileToMerge;
}
