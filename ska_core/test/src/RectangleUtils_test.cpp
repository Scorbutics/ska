#include <doctest.h>
#include "Core/Utils/RectangleUtils.h"

TEST_CASE("[RectangleUtils] Position dans la box") {

    ska::Rectangle box {0, 4, 14, 24};

    SUBCASE("Cas interieur") {
        ska::Point<int> p(10, 10);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas exterieur") {
        ska::Point<int> p(100, 100);
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites x sup") {
        ska::Point<int> p(13, 17);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.x++;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites x inf") {
        ska::Point<int> p(0, 17);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.x--;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites y sup") {
        ska::Point<int> p(10, 27);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.y++;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites y inf") {
        ska::Point<int> p(10, 4);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.y--;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }
}

TEST_CASE("[RectangleUtils] Union de plusieurs box") {

	ska::Rectangle box{ 0, 4, 14, 24 };

	SUBCASE("Union x rectangles collés") {
		ska::Rectangle box2{ 14, 4, 14, 24 };
		auto resultRect = ska::RectangleUtils::unionRect(box, box2);
		CHECK(resultRect.x == 0);
		CHECK(resultRect.y == 4);
		CHECK(resultRect.w == 28);
		CHECK(resultRect.h == 24);
	}

	SUBCASE("Union y rectangles collés") {
		ska::Rectangle box2{ 0, 28, 14, 24 };
		auto resultRect = ska::RectangleUtils::unionRect(box, box2);
		CHECK(resultRect.x == 0);
		CHECK(resultRect.y == 4);
		CHECK(resultRect.w == 14);
		CHECK(resultRect.h == 48);
	}

	SUBCASE("Union 2 rectangles distants") {
		ska::Rectangle box2{ 48, 22, 31, 12 };
		auto resultRect = ska::RectangleUtils::unionRect(box, box2);
		CHECK(resultRect.x == 0);
		CHECK(resultRect.y == 4);
		CHECK(resultRect.w == 79);
		CHECK(resultRect.h == 30);
	}
}

TEST_CASE("[RectangleUtils] Intersection de plusieurs box") {

	ska::Rectangle box{ 0, 0, 100, 50 };

	SUBCASE("Intersection box interne") {
		ska::Rectangle box2{ 10, 10, 50, 10 };
		auto resultRect = ska::RectangleUtils::intersect(box, box2);

		CHECK(resultRect.x == box2.x);
		CHECK(resultRect.y == box2.y);
		CHECK(resultRect.w == box2.w);
		CHECK(resultRect.h == box2.h);
	}

	SUBCASE("Intersection box limite x sup") {
		ska::Rectangle box2{ 99, 0, 1, 1 };
		auto resultRect = ska::RectangleUtils::intersect(box, box2);

		CHECK(resultRect.x == box2.x);
		CHECK(resultRect.y == box2.y);
		CHECK(resultRect.w == 1);
		CHECK(resultRect.h == 1);
	}

	SUBCASE("Intersection box limite x inf") {
		ska::Rectangle box2{ -1, 0, 2, 1 };
		auto resultRect = ska::RectangleUtils::intersect(box, box2);

		CHECK(resultRect.x == box.x);
		CHECK(resultRect.y == box.y);
		CHECK(resultRect.w == 1);
		CHECK(resultRect.h == 1);
	}

	SUBCASE("Intersection box limite y sup") {
		ska::Rectangle box2{ 0, 49, 1, 1 };
		auto resultRect = ska::RectangleUtils::intersect(box, box2);
		
		CHECK(resultRect.x == box2.x);
		CHECK(resultRect.y == box2.y);
		CHECK(resultRect.w == 1);
		CHECK(resultRect.h == 1);
	}

	SUBCASE("Pas d'intersection, hors de box") {
		ska::Rectangle box2{ 100, 10, 10, 10 };
		auto resultRect = ska::RectangleUtils::intersect(box, box2);

		CHECK(resultRect.x == 0);
		CHECK(resultRect.y == 0);
		CHECK(resultRect.w == 0);
		CHECK(resultRect.h == 0);
	}

	SUBCASE("Pas d'intersection, 1 box sans largeur") {
		ska::Rectangle box2{ 10, 10, 0, 10 };
		auto resultRect = ska::RectangleUtils::intersect(box, box2);

		CHECK(resultRect.x == 0);
		CHECK(resultRect.y == 0);
		CHECK(resultRect.w == 0);
		CHECK(resultRect.h == 0);
	}

	SUBCASE("Pas d'intersection, 1 box sans hauteur") {
		ska::Rectangle box2{ 10, 10, 10, 0 };
		auto resultRect = ska::RectangleUtils::intersect(box, box2);

		CHECK(resultRect.x == 0);
		CHECK(resultRect.y == 0);
		CHECK(resultRect.w == 0);
		CHECK(resultRect.h == 0);
	}
}