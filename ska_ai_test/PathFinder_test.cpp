#include <doctest.h>
#include "AI/PathFinder.h"
#include <iostream>

void DisplayCollisions(const ska::Vector2<char>& collisions) {
	static constexpr auto width = 10u;
	static constexpr auto height = 10u;

	for(auto y = 0u; y < height; y++) {
		for(auto x = 0u; x < width; x++) {
			std::cout << collisions[x][y] << " ";
		}	
		std::cout << std::endl;
	}
}

TEST_CASE("[PathFinder]") {

	SUBCASE("Collision Map") {
		
		constexpr auto width = 10u;

		auto collisions = ska::Vector2<char> { width, 
		   {0, 0,  0,   0,  0, '#', '#', 0, 0, 0,
			0, 0,  0,   0,  0,  0,  '#', 0, 0, 0,
			0, 0, '#', '#', 0, '#', '#', 0, 0, 0,
			0, 0, '#', '#', 0, '#',  0,  0, 0, 0,
			0, 0, '#',  0,  0, '#',  0,  0, 0, 0,
			0, 0, '#', '#', 0, '#',  0,  0, 0, 0,
			0, 0, '#', '#', 0,  0,   0,  0, 0, 0,
			0, 0, '#', '#', 0,  0,   0,  0, 0, 0,
			0, 0, '#', '#', 0,  0,   0,  0, 0, 0,
			0, 0,  0,  '#', 0,  0,   0,  0, 0, 0 }};

		auto pathFinder = ska::PathFinder { collisions };

		SUBCASE("Basic") {
			const auto from = ska::Point<int> { 0, 8 };
			const auto to = ska::Point<int> { 8, 2 };
			const auto p = pathFinder.findPath(from, to);
			collisions[from.x][from.y] = 'S';
			collisions[to.x][to.y] = 'G';
			auto currentPosition = ska::Point<int> {from};
			for(const auto& node : p.path) {
				switch(node) {
				case ska::PathDirection::Down: currentPosition.y++; break;
				case ska::PathDirection::Right: currentPosition.x++; break;
				case ska::PathDirection::Up: currentPosition.y--; break;
				case ska::PathDirection::Left: currentPosition.x--; break;
				case ska::PathDirection::Unknown: break;
				default: ;
				}
				if(currentPosition.y >= 0 && currentPosition.x >=0 && currentPosition.x < 10 && currentPosition.y < 10) {
					collisions[currentPosition.x][currentPosition.y] = '.';
				}
			}
			DisplayCollisions(collisions);
			//CHECK();
		}
	}


	
}