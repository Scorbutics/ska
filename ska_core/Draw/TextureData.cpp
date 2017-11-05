#include "TextureData.h"

ska::TextureData::TextureData(SDLRenderer& r, const std::string& texturePath, Color col, const ska::Rectangle& trect, EnumTextureType ttype, unsigned int fs) :
	m_renderer(&r), type(ttype),
	fontSize(fs),
	rect(trect) {
	m_data = std::make_pair(texturePath, col);
}

ska::TextureData::TextureData() :
	m_renderer(nullptr), type(SPRITE),
	fontSize(0), rect{ 0, 0, 0, 0 } {
}

const ska::SDLRenderer& ska::TextureData::getRenderer() const {
	return *m_renderer;
}

std::pair<std::string, ska::Color> ska::TextureData::getData() const {
	return m_data;
}
