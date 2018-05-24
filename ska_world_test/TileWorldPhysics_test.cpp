#include <doctest.h>
#include <fakeit.hpp>
#include "World/TilesetEvent.h"
#include "World/Layer.h"
#include "World/CollisionProfile.h"
#include "World/TileWorldPhysics.h"

namespace TileWorldPhysics {
	ska::Vector2<std::optional<ska::Tile>> BuildTiles(const ska::Vector2<char>& collisions, const ska::Vector2<int>& properties) {

		const auto width = collisions.lineSize();
		const auto height = collisions.size() / collisions.lineSize();

		auto tiles = ska::Vector2<std::optional<ska::Tile>>{};
		tiles.resize(width, height);

		for (auto x = 0; x < width; x++) {
			for (auto y = 0; y < height; y++) {
				auto tileProperties = ska::TileProperties{};
				tileProperties.bitMask = !properties.has(x, y) ? 0 : properties[x][y];
				tiles[x][y] = ska::Tile{ { x, y }, tileProperties,
					collisions[x][y] == 0 ? ska::TileCollision::No :
					(collisions[x][y] == '-' ? ska::TileCollision::Void : ska::TileCollision::Yes)
				};
			}
		}

		return tiles;
	}

	ska::CollisionProfile BuildCollisionProfile(const ska::Vector2<char>& collisions, const ska::Vector2<int>& properties = ska::Vector2<int>{}) {
		const auto tiles = TileWorldPhysics::BuildTiles(collisions, properties);
		auto layers = std::vector<ska::LayerPtr>{};
		layers.emplace_back(std::make_unique<ska::Layer>(tiles));
		return ska::CollisionProfile{ 1, std::move(layers) };
	}

}

TEST_CASE("[TileWorldPhysics] GenerateTileMap") {

	SUBCASE("1 tile 1 collision") {
		static constexpr auto width = 1u;
		const auto collisions = ska::Vector2<char>{ width, { '#' } };
		const auto collisionProfile = TileWorldPhysics::BuildCollisionProfile(collisions);
		auto tileMap = GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(tileMap.size() == 1);
		CHECK(tileMap[0].pointList.size() == 4);

		bool checkPoint[4];
		checkPoint[0] = tileMap[0].pointList.front() == ska::Point<int>{0, 0};
		tileMap[0].pointList.pop_front();
		checkPoint[1] = tileMap[0].pointList.front() == ska::Point<int>{0, 1};
		tileMap[0].pointList.pop_front();
		checkPoint[2] = tileMap[0].pointList.front() == ska::Point<int>{1, 1};
		tileMap[0].pointList.pop_front();
		checkPoint[3] = tileMap[0].pointList.front() == ska::Point<int>{1, 0};

		for (const auto check : checkPoint) {
			CHECK(check);
		}
	}

	SUBCASE("1 tile 0 collision") {
		static constexpr auto width = 1u;
		const auto collisions = ska::Vector2<char>{ width, { 0 } };
		const auto collisionProfile = TileWorldPhysics::BuildCollisionProfile(collisions);
		auto tileMap = GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(tileMap.empty());
	}

	SUBCASE("10x10 tiles 100 collisions") {
		static constexpr auto width = 10u;
		const auto collisions = ska::Vector2<char>{ width,
		 {  '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
		    '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', } };

		const auto collisionProfile = TileWorldPhysics::BuildCollisionProfile(collisions);
		auto tileMap = GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(tileMap.size() == 1);
		CHECK(tileMap[0].pointList.size() == 40);

		auto collisionPath = std::vector<ska::Point<int>> {};
		collisionPath.reserve(tileMap[0].pointList.size());
		std::copy(std::begin(tileMap[0].pointList), std::end(tileMap[0].pointList), std::back_inserter(collisionPath));

		bool checkPoint[5];
		checkPoint[0] = collisionPath[0] == ska::Point<int>{0, 0};
		checkPoint[1] = collisionPath[10] == ska::Point<int>{0, 10};
		checkPoint[2] = collisionPath[20] == ska::Point<int>{10, 10};
		checkPoint[3] = collisionPath[30] == ska::Point<int>{10, 0};
		checkPoint[4] = collisionPath[39] == ska::Point<int>{1, 0};

		for (const auto check : checkPoint) {
			CHECK(check);
		}
	}

	SUBCASE("4x4 tiles 'hollow rectangle'") {
		static constexpr auto width = 4u;
		const auto collisions = ska::Vector2<char>{ width, {
			'#', '#', '#', '#',
			'#',  0,   0,  '#',
			'#',  0,   0,  '#',
			'#', '#', '#', '#'
		} };
		const auto collisionProfile = TileWorldPhysics::BuildCollisionProfile(collisions);
		auto tileMap = GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(tileMap.size() == 2);
		CHECK(tileMap[0].pointList.size() == 16);
		CHECK(tileMap[1].pointList.size() == 8);

		{
			//Anti-horaire (extérieur)
			auto collisionPath = std::vector<ska::Point<int>>{};
			collisionPath.reserve(tileMap[0].pointList.size());
			std::copy(std::begin(tileMap[0].pointList), std::end(tileMap[0].pointList), std::back_inserter(collisionPath));

			auto check = std::vector<bool>{};
			check.push_back(collisionPath[0] == ska::Point<int>{0, 0});
			check.push_back(collisionPath[1] == ska::Point<int>{0, 1});
			check.push_back(collisionPath[2] == ska::Point<int>{0, 2});
			check.push_back(collisionPath[3] == ska::Point<int>{0, 3});
			check.push_back(collisionPath[4] == ska::Point<int>{0, 4});
			check.push_back(collisionPath[5] == ska::Point<int>{1, 4});
			check.push_back(collisionPath[6] == ska::Point<int>{2, 4});
			check.push_back(collisionPath[7] == ska::Point<int>{3, 4});
			check.push_back(collisionPath[8] == ska::Point<int>{4, 4});
			check.push_back(collisionPath[9] == ska::Point<int>{4, 3});
			check.push_back(collisionPath[10] == ska::Point<int>{4, 2});
			check.push_back(collisionPath[11] == ska::Point<int>{4, 1});
			check.push_back(collisionPath[12] == ska::Point<int>{4, 0});
			check.push_back(collisionPath[13] == ska::Point<int>{3, 0});
			check.push_back(collisionPath[14] == ska::Point<int>{2, 0});
			check.push_back(collisionPath[15] == ska::Point<int>{1, 0});

			for (const auto& c : check) {
				CHECK(c);
			}
		}
		{
			//Anti-horaire (intérieur)
			auto collisionPath = std::vector<ska::Point<int>>{};
			collisionPath.reserve(tileMap[1].pointList.size());
			std::copy(std::begin(tileMap[1].pointList), std::end(tileMap[1].pointList), std::back_inserter(collisionPath));

			auto check = std::vector<bool>{};
			check.push_back(collisionPath[0] == ska::Point<int>{1, 3});
			check.push_back(collisionPath[1] == ska::Point<int>{1, 2});
			check.push_back(collisionPath[2] == ska::Point<int>{1, 1});
			check.push_back(collisionPath[3] == ska::Point<int>{2, 1});
			check.push_back(collisionPath[4] == ska::Point<int>{3, 1});
			check.push_back(collisionPath[5] == ska::Point<int>{3, 2});
			check.push_back(collisionPath[6] == ska::Point<int>{3, 3});
			check.push_back(collisionPath[7] == ska::Point<int>{2, 3});

			for (const auto& c : check) {
				CHECK(c);
			}
		}
	}

	SUBCASE("5x5 tiles 'hollow rectangle with center'") {
		static constexpr auto width = 5u;
		const auto collisions = ska::Vector2<char>{ width,{
			'#', '#', '#', '#', '#',
			'#',  0,   0,   0,  '#',
			'#',  0,  '#',  0,  '#',
			'#',  0,   0,   0,  '#',
			'#', '#', '#', '#', '#'
		} };
		const auto collisionProfile = TileWorldPhysics::BuildCollisionProfile(collisions);
		auto tileMap = GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(tileMap.size() == 3);
		CHECK(tileMap[0].pointList.size() == 20);
		CHECK(tileMap[1].pointList.size() == 12);
		CHECK(tileMap[2].pointList.size() == 4);

	}

	SUBCASE("4x4 tiles 'hollow donut'") {
		static constexpr auto width = 4u;
		const auto collisions = ska::Vector2<char>{ width,{
			 0,  '#', '#',  0,
			'#',  0,   0,  '#',
			'#',  0,   0,  '#',
			 0,  '#', '#',  0
		} };
		const auto collisionProfile = TileWorldPhysics::BuildCollisionProfile(collisions);
		auto tileMap = GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(tileMap.size() == 4);
		CHECK(tileMap[0].pointList.size() == 6);
		CHECK(tileMap[1].pointList.size() == 6);
		CHECK(tileMap[2].pointList.size() == 6);
		CHECK(tileMap[3].pointList.size() == 6);

		{
			auto collisionPath = std::vector<ska::Point<int>>{};
			collisionPath.reserve(tileMap[0].pointList.size());
			std::copy(std::begin(tileMap[0].pointList), std::end(tileMap[0].pointList), std::back_inserter(collisionPath));

			auto check = std::vector<bool>{};
			check.push_back(collisionPath[0] == ska::Point<int>{0, 1});
			check.push_back(collisionPath[1] == ska::Point<int>{0, 2});
			check.push_back(collisionPath[2] == ska::Point<int>{0, 3});
			check.push_back(collisionPath[3] == ska::Point<int>{1, 3});
			check.push_back(collisionPath[4] == ska::Point<int>{1, 2});
			check.push_back(collisionPath[5] == ska::Point<int>{1, 1});

			for (const auto& c : check) {
				CHECK(c);
			}
		}
		{
			auto collisionPath = std::vector<ska::Point<int>>{};
			collisionPath.reserve(tileMap[1].pointList.size());
			std::copy(std::begin(tileMap[1].pointList), std::end(tileMap[1].pointList), std::back_inserter(collisionPath));

			auto check = std::vector<bool>{};
			check.push_back(collisionPath[0] == ska::Point<int>{1, 0});
			check.push_back(collisionPath[1] == ska::Point<int>{1, 1});
			check.push_back(collisionPath[2] == ska::Point<int>{2, 1});
			check.push_back(collisionPath[3] == ska::Point<int>{3, 1});
			check.push_back(collisionPath[4] == ska::Point<int>{3, 0});
			check.push_back(collisionPath[5] == ska::Point<int>{2, 0});

			for (const auto& c : check) {
				CHECK(c);
			}
		}
		{
			auto collisionPath = std::vector<ska::Point<int>>{};
			collisionPath.reserve(tileMap[2].pointList.size());
			std::copy(std::begin(tileMap[2].pointList), std::end(tileMap[2].pointList), std::back_inserter(collisionPath));

			auto check = std::vector<bool>{};
			check.push_back(collisionPath[0] == ska::Point<int>{2, 3});
			check.push_back(collisionPath[1] == ska::Point<int>{1, 3});
			check.push_back(collisionPath[2] == ska::Point<int>{1, 4});
			check.push_back(collisionPath[3] == ska::Point<int>{2, 4});
			check.push_back(collisionPath[4] == ska::Point<int>{3, 4});
			check.push_back(collisionPath[5] == ska::Point<int>{3, 3});

			for (const auto& c : check) {
				CHECK(c);
			}
		}
		{
			auto collisionPath = std::vector<ska::Point<int>>{};
			collisionPath.reserve(tileMap[3].pointList.size());
			std::copy(std::begin(tileMap[3].pointList), std::end(tileMap[3].pointList), std::back_inserter(collisionPath));

			auto check = std::vector<bool>{};
			check.push_back(collisionPath[0] == ska::Point<int>{3, 1});
			check.push_back(collisionPath[1] == ska::Point<int>{3, 2});
			check.push_back(collisionPath[2] == ska::Point<int>{3, 3});
			check.push_back(collisionPath[3] == ska::Point<int>{4, 3});
			check.push_back(collisionPath[4] == ska::Point<int>{4, 2});
			check.push_back(collisionPath[5] == ska::Point<int>{4, 1});

			for (const auto& c : check) {
				CHECK(c);
			}
		}
	}

	SUBCASE("4x4 tiles no collision") {
		static constexpr auto width = 4u;
		const auto collisions = ska::Vector2<char>{ width,{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		} };
		const auto collisionProfile = TileWorldPhysics::BuildCollisionProfile(collisions);
		auto tileMap = GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(tileMap.empty());
	}

	SUBCASE("Complex") {
		static constexpr auto width = 10u;

		const auto collisions = ska::Vector2<char>{ width,
		 { '#','#', 0,   0,   0, '#', '#', 0, 0, 0,
		   '#', 0,  0,   0,   0,  0,  '#', 0, 0, 0,
			0,  0, '#', '#',  0, '#', '#', 0, 0, 0,
			0,  0, '#', '#',  0, '#',  0,  0, 0, 0,
			0,  0, '#',  0,   0, '#',  0,  0, 0, 0,
			0,  0, '#', '#',  0, '#',  0,  0, 0, 0,
			0,  0, '#', '#',  0,  0,   0,  0, 0, 0,
			0,  0, '#', '#',  0,  0,   0,  0, 0, 0,
			0,  0, '#', '#',  0,  0,   0,  0, 0, 0,
			0,  0,  0,  '#',  0,  0,   0,  0, 0, 0 } };

		const auto tiles = TileWorldPhysics::BuildTiles(collisions, ska::Vector2<int>{});
		auto layers = std::vector<ska::LayerPtr>{};
		layers.emplace_back(std::make_unique<ska::Layer>(tiles));
		const auto collisionProfile = ska::CollisionProfile{ 1, std::move(layers) };

		auto tileMap = ska::GenerateAgglomeratedTileMap(0, collisionProfile);

		CHECK(!tileMap.empty());
	}

}

TEST_CASE("[TileWorldPhysics] GenerateContourTileMap") {

	SUBCASE("4x4 tiles 'hollow rectangle'") {
		auto points = std::vector<ska::Point<int>>{};
		points.emplace_back(0, 0);
		points.emplace_back(0, 10);
		points.emplace_back(0, 20);
		points.emplace_back(0, 30);
		points.emplace_back(0, 40);
		points.emplace_back(10, 40);
		points.emplace_back(20, 40);
		points.emplace_back(30, 40);
		points.emplace_back(40, 40);
		points.emplace_back(40, 30);
		points.emplace_back(40, 20);
		points.emplace_back(40, 10);
		points.emplace_back(40, 0);
		points.emplace_back(30, 0);
		points.emplace_back(20, 0);
		points.emplace_back(10, 0);

		auto pointArea = ska::PointArea{};
		pointArea.pointList.insert(std::end(pointArea.pointList), std::begin(points), std::end(points));

		auto contourMap = ska::GenerateContourTileMap(std::vector<ska::PointArea>{pointArea});
		CHECK(!contourMap.empty());
	}
}

TEST_CASE("[TileWorldPhysics] GenerateAgglomeratedTileMap : Custom predicate") {

	SUBCASE("4x4 tiles") {
		static constexpr auto width = 2u;

		const auto collisions = ska::Vector2<char>{ width,
		 { '#','#',
		   '#', 0 } };

		const auto properties = ska::Vector2<int>{ width,
		  { 1, 1, 
		    0, 0 } };

		auto cp = TileWorldPhysics::BuildCollisionProfile(collisions, properties);
		
		auto contourMap = ska::GenerateAgglomeratedTileMap(0, std::move(cp), [](const ska::Tile* t) {
			if(t == nullptr) {
                return ska::TileCollision::No;
            }
			return t->properties.bitMask == 1 ? ska::TileCollision::Yes : ska::TileCollision::No;
		});

		auto pointsExpected = std::vector<ska::Point<int>> {};
		pointsExpected.push_back(ska::Point<int>{0,0});
		pointsExpected.push_back(ska::Point<int>{0,1});
		pointsExpected.push_back(ska::Point<int>{1,1});
		pointsExpected.push_back(ska::Point<int>{2,1});
		pointsExpected.push_back(ska::Point<int>{2,0});
		pointsExpected.push_back(ska::Point<int>{1,0});

		CHECK(contourMap.size() == 1);
		CHECK(contourMap[0].pointList.size() == pointsExpected.size());

		auto index = 0u;
		for(const auto& p : contourMap[0].pointList) {
            CHECK(p == pointsExpected[index++]);
		}
	}

	SUBCASE("4x4 tiles, no collisions on bitmask") {
		static constexpr auto width = 2u;

		const auto collisions = ska::Vector2<char>{ width,
		  {  0,  0,
			'#', 0 } };

		const auto properties = ska::Vector2<int>{ width,
		  { 1, 1,
			0, 0 } };

		auto cp = TileWorldPhysics::BuildCollisionProfile(collisions, properties);

		auto contourMap = ska::GenerateAgglomeratedTileMap(0, std::move(cp), [](const ska::Tile* t) {
			if (t == nullptr) {
				return ska::TileCollision::No;
			}
			return t->properties.bitMask == 1 ? ska::TileCollision::Yes : ska::TileCollision::No;
		});


		auto pointsExpected = std::vector<ska::Point<int>>{};
		pointsExpected.push_back(ska::Point<int>{0, 0});
		pointsExpected.push_back(ska::Point<int>{0, 1});
		pointsExpected.push_back(ska::Point<int>{1, 1});
		pointsExpected.push_back(ska::Point<int>{2, 1});
		pointsExpected.push_back(ska::Point<int>{2, 0});
		pointsExpected.push_back(ska::Point<int>{1, 0});

		CHECK(contourMap.size() == 1);
		CHECK(contourMap[0].pointList.size() == pointsExpected.size());

		auto index = 0u;
		for (const auto& p : contourMap[0].pointList) {
			CHECK(p == pointsExpected[index++]);
		}
	}

	SUBCASE("10x10 tiles") {
		static constexpr auto width = 10u;

		const auto collisions = ska::Vector2<char>{ width,
		 { '#','#', 0,   0,   0, '#', '#', 0, 0, 0,
		   '#', 0,  0,   0,   0,  0,  '#', 0, 0, 0,
			0,  0, '#', '#',  0, '#', '#', 0, 0, 0,
			0,  0, '#', '#',  0, '#',  0,  0, 0, 0,
			0,  0, '#', '#',  0, '#',  0,  0, 0, 0,
			0,  0, '#', '#',  0, '#',  0,  0, 0, 0,
			0,  0, '#', '#',  0,  0,   0,  0, 0, 0,
			0,  0, '#', '#',  0,  0,   0,  0, 0, 0,
			0,  0, '#', '#',  0,  0,   0,  0, 0, 0,
			0,  0,  0,  '#',  0,  0,   0,  0, 0, 0 } };

		const auto properties = ska::Vector2<int>{ width,
		  { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
			0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
			0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
			0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
			0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

		auto cp = TileWorldPhysics::BuildCollisionProfile(collisions, properties);

		auto contourMap = ska::GenerateAgglomeratedTileMap(0, std::move(cp), [] (const ska::Tile* t) {
			if (t == nullptr) {
				return ska::TileCollision::No;
			}
			return t->properties.bitMask == 1 ? ska::TileCollision::Yes : ska::TileCollision::No;
		});

		CHECK(contourMap.size() == 2);
		CHECK(contourMap[0].pointList.size() == 6);
		CHECK(contourMap[1].pointList.size() == 14);
	}
}
