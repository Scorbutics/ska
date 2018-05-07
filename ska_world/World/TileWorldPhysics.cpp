#include <unordered_set>

#include "TileAgglomerate.h"
#include "MarchingSquare.h"
#include "Utils/RectangleUtils.h"
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

std::vector<ska::PointArea> ska::GenerateAgglomeratedTileMap(const std::size_t layerMax, const CollisionProfile& world) {
	std::vector<PointArea> result;
	std::unordered_set<Point<int>> remainingBlocks;

	Point<int> lastStartPoint = {};

	const int width = world.getBlocksX();
	const int height = world.getBlocksY();

	do {
		PointArea pointList{};
		std::tie(lastStartPoint, pointList.pointList) = MarchingSquare(layerMax, world, remainingBlocks, [](const Tile* b) {
			return b != nullptr ? b->collision : TileCollision::No;
		}, lastStartPoint);
		
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
			contours.push_back(std::move(rect));
		}
	}

	return contours;
}

std::vector<ska::Rectangle> ska::detail::GenerateAgglomeratedTileMapBasic(const std::size_t layerMax, const ska::CollisionProfile& world) {
	return TileAgglomerate(layerMax, world);
}