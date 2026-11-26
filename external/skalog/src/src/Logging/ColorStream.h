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
		WHITE,
		BKGD_BLACK,
		BKGD_BLUE,
		BKGD_GREEN,
		BKGD_CYAN,
		BKGD_RED,
		BKGD_MAGENTA,
		BKGD_YELLOW,
		BKGD_WHITE
	};

	std::ostream& operator<<(std::ostream& stream, const EnumColorStream &);
}
