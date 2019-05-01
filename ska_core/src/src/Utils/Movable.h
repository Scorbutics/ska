#pragma once

namespace ska {
	struct Movable {
		Movable() = default;
		Movable(Movable&&) = default;
		Movable& operator=(Movable&&) = default;
	};
}