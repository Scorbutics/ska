#include <unordered_set>
#include <map>

#include "TileAgglomerate.h"
#include "Core/Utils/RectangleUtils.h"
#include "TileWorldPhysics.h"
#include "CollisionProfile.h"

namespace ska {
	namespace detail {
		enum class Align {
			Horizontal,
			Vertical,
			None
		};

		Align GetPointAlignment(const Point<int>& currentPoint, const Point<int>& lastPoint);
		std::vector<PointArea> FilterAlignedSuccessivePoints(const std::vector<PointArea>& areas);
		std::vector<Rectangle> GenerateAgglomeratedTileMapBasic(std::size_t layerMax, const CollisionProfile & world);
		Rectangle GenerateRect(const Point<int>& start, const Point<int>& end);
	}
}

std::vector<ska::PointArea> ska::GenerateAgglomeratedTileMap(const std::size_t layerMax, const CollisionProfile& world, const MarchingSquarePredicate& pred) {
	std::vector<PointArea> result;
	std::unordered_set<Point<int>> remainingBlocks;

	Point<int> lastStartPoint = {};

	const int width = world.getBlocksX();
	const int height = world.getBlocksY();

	do {
		PointArea pointList{};
		std::tie(lastStartPoint, pointList.pointList) = MarchingSquare(layerMax, world, remainingBlocks, pred, lastStartPoint);
		
		if (!pointList.pointList.empty()) {
			result.push_back(pointList);
		}
	} while (lastStartPoint.x != width || lastStartPoint.y != height);

	return result;
}

ska::Rectangle ska::detail::GenerateRect(const Point<int>& start, const Point<int>& end) {
	static constexpr auto depth = 3;
	auto rect = ska::RectangleUtils::createRectangleFromPoints(start, end);
	if (rect.w == 0) {
		rect.w = depth;
		rect.x -= depth / 2;
		rect.y += depth / 2;
		rect.h -= depth / 2;
	}
	if (rect.h == 0) {
		rect.h = depth;
		rect.y -= depth / 2;
		rect.x += depth / 2;
		rect.w -= depth / 2;
	}
	return rect;
}

ska::detail::Align ska::detail::GetPointAlignment(const Point<int>& currentPoint, const Point<int>& lastPoint) {
	const auto sameDirectionY = currentPoint.x == lastPoint.x && currentPoint.y != lastPoint.y;
	const auto sameDirectionX = currentPoint.y == lastPoint.y && currentPoint.x != lastPoint.x;
	return sameDirectionX ? Align::Horizontal : (sameDirectionY ? Align::Vertical : Align::None);
}

std::vector<ska::PointArea> ska::detail::FilterAlignedSuccessivePoints(const std::vector<ska::PointArea>& areas) {
	std::vector<ska::PointArea> filteredPointAreas;
	filteredPointAreas.reserve(areas.size());

	for (const auto& pa : areas) {
		if (pa.pointList.size() < 2) {
			continue;
		}

		auto pointArea = ska::PointArea{};

		const auto startPointIt = pa.pointList.cbegin();
		auto nextToStartPoint = startPointIt;
		++nextToStartPoint;
		auto lastPointIt = startPointIt;
		auto lastAlign = Align::None;

		for (auto pIt = nextToStartPoint; pIt != pa.pointList.cend(); lastPointIt = pIt, ++pIt) {
			const auto pointAlign = GetPointAlignment(*lastPointIt, *pIt);
			if (pointAlign != lastAlign && pointAlign != Align::None) {
				pointArea.pointList.emplace_back(*lastPointIt);

				lastAlign = pointAlign;
			}
		}

		const auto pointAlign = GetPointAlignment(*lastPointIt, *startPointIt);
		if (pointAlign != lastAlign && pointAlign != Align::None) {
			pointArea.pointList.emplace_back(*lastPointIt);
		}

		pointArea.pointList.emplace_back(*startPointIt);

		filteredPointAreas.push_back(std::move(pointArea));

	}
	return filteredPointAreas;
}

std::vector<ska::Rectangle> ska::GenerateContourTileMap(const std::vector<PointArea>& areas) {
	const auto filteredPointAreas = detail::FilterAlignedSuccessivePoints(areas);

	std::vector<ska::Rectangle> contours;
	for (const auto& pa : filteredPointAreas) {
		auto lastPointIt = pa.pointList.cbegin();
		for (auto pIt = lastPointIt; pIt != pa.pointList.cend(); lastPointIt = pIt, ++pIt) {
			auto rect = detail::GenerateRect(*lastPointIt, *pIt);
			if (rect.h > 0 && rect.w > 0) {
				contours.push_back(std::move(rect));
			}
		}
	}

	return contours;
}

std::vector<ska::Rectangle> ska::detail::GenerateAgglomeratedTileMapBasic(const std::size_t layerMax, const ska::CollisionProfile& world) {
	return TileAgglomerate(layerMax, world);
}
std::vector<ska::Rectangle> ska::GenerateFilledTileMap(const std::size_t layerMax, const CollisionProfile& world, const MarchingSquarePredicate& pred) {
	const auto width = static_cast<int>(world.getBlocksX());
	const auto height = static_cast<int>(world.getBlocksY());
	const auto blockSize = static_cast<int>(world.getBlockSize());

	std::vector<Rectangle> result;
	// The rectangles still growing down, by their columns (first block, number of blocks)
	auto open = std::map<std::pair<int, int>, Rectangle>{};
	for (auto y = 0; y <= height; y++) {
		auto next = std::map<std::pair<int, int>, Rectangle>{};
		auto x = 0;
		while (y < height && x < width) {
			if (pred(world.getHighestBlockOnPredicate(layerMax, x, y, pred)) != TileCollision::Yes) {
				x++;
				continue;
			}
			const auto start = x;
			while (x < width && pred(world.getHighestBlockOnPredicate(layerMax, x, y, pred)) == TileCollision::Yes) {
				x++;
			}
			const auto columns = std::make_pair(start, x - start);
			const auto above = open.find(columns);
			if (above != open.end()) {
				above->second.h += blockSize;
				next.emplace(columns, above->second);
				open.erase(above);
			} else {
				next.emplace(columns, Rectangle{ start * blockSize, y * blockSize, (x - start) * blockSize, blockSize });
			}
		}
		// The rectangles not continued on this line are finished
		for (const auto& finished : open) {
			result.push_back(finished.second);
		}
		open = std::move(next);
	}
	return result;
}
