#include "WidgetMaskHelper.h"

unsigned int ska::WStaticCounterGlobal::getCounterAndInc() {
	static std::size_t id = 0;
	return id++;
}
