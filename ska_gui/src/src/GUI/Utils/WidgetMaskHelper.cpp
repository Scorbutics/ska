#include "WidgetMaskHelper.h"

unsigned int ska::WStaticCounterGlobal::getCounterAndInc() {
	static unsigned int m_typeCounter = 0;
	return m_typeCounter++;
}