#include "TextureData.h"

ska::TextureData::TextureData(const std::string& texturePath, Color col, const ska::Rectangle& trect, EnumTextureType ttype, unsigned int fontSiz, const Color* outlineCol) :
	type(ttype),
	fontSize(fontSiz),
	rect(trect) {
	m_data.text = texturePath;
	m_data.color = std::move(col);
	m_data.outlineColor = outlineCol;
}

ska::TextureData::TextureData() :
	type(EnumTextureType::SPRITE),
	fontSize(0), 
	rect{ 0, 0, 0, 0 } {
}

const ska::TextureDataDescriptor& ska::TextureData::getData() const {
	return m_data;
}

bool ska::operator==(const ska::TextureDataDescriptor & t1, const ska::TextureDataDescriptor & t2) {
	return t1.color == t2.color && t1.outlineColor == t2.outlineColor && t1.text == t2.text;	
}

