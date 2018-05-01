#pragma once
#include "Point.h"

namespace ska {
	struct BlockEvent {
		Point<int> position;

		int trigger{};

		std::string param;
		std::string path;
		std::string script;
	};
}
