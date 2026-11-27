#pragma once

namespace ska {
	// The row of a direction (AnimationComponent::state) in a character sheet : 0 down, 1 right, 2 up, 3 left, 4 down
	// right, 5 up right, 6 up left, 7 down left ; the rows : up, right, down, left, up left, up right, down left, down right.
	// -1 : not a direction
	inline int CharsetRow(long state) {
		constexpr int rows[] = { 2, 1, 0, 3, 7, 5, 4, 6 };
		return state >= 0 && state < 8 ? rows[state] : -1;
	}
}
