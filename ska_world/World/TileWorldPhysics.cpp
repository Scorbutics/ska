#include <unordered_set>

#include "TileAgglomerate.h"
#include "MarchingSquare.h"
#include "Utils/RectangleUtils.h"
#include "TileWorldPhysics.h"

std::vector<ska::PointArea> ska::GenerateAgglomeratedTileMap(const ska::TileWorld& world) {
	std::vector<ska::PointArea> result;
	std::unordered_set<ska::Point<int>> remainingBlocks;

	auto done = false;
	do {
		PointArea pointList{};
		std::tie(done, pointList.pointList) = ska::MarchingSquare(world, remainingBlocks, [](const ska::Tile* b) {
			return b != nullptr ? b->collision : ska::TileCollision::No;
		});
		result.push_back(pointList);
	} while (!done);

	return result;
}

ska::Rectangle GenerateRect(const ska::Point<int>& start, const ska::Point<int>& end) {
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

enum class Align {
	Horizontal,
	Vertical,
	None
};

Align GetPointAlignment(const ska::Point<int>& currentPoint, const ska::Point<int>& lastPoint) {
	const auto sameDirectionY = currentPoint.x == lastPoint.x && currentPoint.y != lastPoint.y;
	const auto sameDirectionX = currentPoint.y == lastPoint.y && currentPoint.x != lastPoint.x;
	return sameDirectionX ? Align::Horizontal : (sameDirectionY ? Align::Vertical : Align::None);
}

std::vector<ska::PointArea> FilterAlignedSuccessivePoints(const std::vector<ska::PointArea>& areas) {
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
	const auto filteredPointAreas = FilterAlignedSuccessivePoints(areas);

	std::vector<ska::Rectangle> contours;
	for (const auto& pa : filteredPointAreas) {
		auto lastPointIt = pa.pointList.cbegin();
		for (auto pIt = lastPointIt; pIt != pa.pointList.cend(); lastPointIt = pIt, ++pIt) {
			auto rect = GenerateRect(*lastPointIt, *pIt);
			contours.push_back(std::move(rect));
		}
	}

	return contours;
}

std::vector<ska::Rectangle> GenerateAgglomeratedTileMapBasic(const ska::TileWorld& world) {
	return TileAgglomerate(world);
}