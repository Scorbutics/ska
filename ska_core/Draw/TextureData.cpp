#include "TextureData.h"

ska::TextureData::TextureData(const std::string& texturePath, Color col, const ska::Rectangle& trect, EnumTextureType ttype, unsigned int fs) :
	type(ttype),
	fontSize(fs),
	rect(trect) {
	m_data = std::make_pair(texturePath, col);
}

ska::TextureData::TextureData() :
	type(SPRITE),
	fontSize(0), rect{ 0, 0, 0, 0 } {
}

std::pair<std::string, ska::Color> ska::TextureData::getData() const {
	return m_data;
}
