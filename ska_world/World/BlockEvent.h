#pragma once
#include "Point.h"

namespace ska {
	struct BlockEvent {
		Point<int> position;

		int id;
		int trigger;
		int solid;

		std::string action;
		std::string param;
		std::string path;
	};
}
