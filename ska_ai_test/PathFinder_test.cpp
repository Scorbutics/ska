#include <doctest.h>
#include "AI/PathFinder.h"

TEST_CASE("[TaskQueue]"){
	ska::Vector2<int> collisions;


	ska::PathFinder taskQueue { collisions };
	
	ska::Point<int> from;
	ska::Point<int> to;

	SUBCASE("Basic") {
		taskQueue.findPath(from, to);
		//CHECK();
	}
}