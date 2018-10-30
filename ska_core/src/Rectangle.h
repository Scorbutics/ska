#pragma once
#include "Utils/NumberUtils.h"

namespace ska {
	struct Rectangle {
		int x, y;
		int w, h;		

		bool operator==(const Rectangle& r) const {
			return !operator!=(r);
		}

		bool operator!=(const Rectangle& r) const {
			return x != r.x || y != r.y || w != r.w || h != r.h;
		}
	};

}

namespace std {
	template <>
	struct hash<ska::Rectangle> {
		size_t operator()(const ska::Rectangle& r) const {
			using std::size_t;
			using std::hash;

			auto hashSeed = hash<int>()(r.x);
			ska::NumberUtils::hashCombine<int>(hashSeed, r.y);
			ska::NumberUtils::hashCombine<int>(hashSeed, r.w);
			ska::NumberUtils::hashCombine<int>(hashSeed, r.h);
			return hashSeed;
		}
	};

	template <>
	struct equal_to<ska::Rectangle> {
		bool operator()(const ska::Rectangle& r1, const ska::Rectangle& r2) const {
			return r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h;
		}
	};
}
