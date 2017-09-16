#include "TextureData.h"
#include "../Utils/StringUtils.h"

ska::TextureData::TextureData(SDLRenderer& r, const std::string& texturePath, Color col, bool isText, unsigned int fs) :
    m_renderer(&r),
	text(isText),
	fontSize(fs) {
	m_data = std::make_pair(texturePath, col);
}

ska::TextureData::TextureData() :
    m_renderer(nullptr),
	text(false),
	fontSize(0) {
}

const ska::SDLRenderer& ska::TextureData::getRenderer() const {
	return *m_renderer;
}

std::pair<std::string, ska::Color> ska::TextureData::getData() const {
	return m_data;
}
