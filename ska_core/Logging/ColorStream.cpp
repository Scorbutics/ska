#include "ColorStream.h"
#include "../../external/rlutil/rlutil.h"

std::ostream& ska::operator<<(std::ostream& stream, const ska::EnumColorStream& color) {
	switch (color) {
	case BLACK: rlutil::setColor(rlutil::BLACK);  break;
	case BLUE: rlutil::setColor(rlutil::BLUE); break;
	case GREEN: rlutil::setColor(rlutil::GREEN); break;
	case CYAN: rlutil::setColor(rlutil::CYAN); break;
	case RED: rlutil::setColor(rlutil::RED); break;
	case MAGENTA: rlutil::setColor(rlutil::MAGENTA); break;
	case BROWN: rlutil::setColor(rlutil::BROWN); break;
	case GREY: rlutil::setColor(rlutil::GREY); break;
	case DARKGREY: rlutil::setColor(rlutil::DARKGREY); break;
	case LIGHTBLUE: rlutil::setColor(rlutil::LIGHTBLUE); break;
	case LIGHTGREEN: rlutil::setColor(rlutil::LIGHTGREEN); break;
	case LIGHTCYAN: rlutil::setColor(rlutil::LIGHTCYAN); break;
	case LIGHTRED: rlutil::setColor(rlutil::LIGHTRED); break;
	case LIGHTMAGENTA: rlutil::setColor(rlutil::LIGHTMAGENTA); break;
	case YELLOW: rlutil::setColor(rlutil::YELLOW); break;
	case WHITE: rlutil::setColor(rlutil::WHITE); break;
	default: ;
	}
	return stream;
}

