#pragma once

namespace ska {
	struct Rectangle {
		int x, y;
		int w, h;

		bool operator==(const Rectangle& r) const {
			return x == r.x && y == r.y && w == r.w && h == r.h;
		}

		bool operator!=(const Rectangle& r) const {
			return x != r.x || y != r.y || w != r.w || h != r.h;
		}
	};
}
