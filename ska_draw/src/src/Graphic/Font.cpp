#include "Font.h"

ska::Font::Font(unsigned int fontSize) : ManagedResource(fontSize) {
}

TTF_Font* ska::Font::getInstance() const {
	return m_value->getInstance();
}
