#pragma once
#include <ostream>

namespace ska {
	enum EnumColorStream {
		BLACK,
		BLUE,
		GREEN,
		CYAN,
		RED,
		MAGENTA,
		BROWN,
		GREY,
		DARKGREY,
		LIGHTBLUE,
		LIGHTGREEN,
		LIGHTCYAN,
		LIGHTRED,
		LIGHTMAGENTA,
		YELLOW,
		WHITE
	};

	std::ostream& operator<<(std::ostream& stream, const EnumColorStream &);
}
