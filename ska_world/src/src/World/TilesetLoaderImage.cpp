#include "World/__internalConfig/LoggerConfig.h"
#include "Core/Exceptions/FileException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "TilesetLoaderImage.h"
#include "Base/Values/AllEquals.h"
#include "Base/Containers/Vector2.h"
#include "Graphic/Texture.h"
#include "Graphic/Animation.h"
#include "Core/SDLLibrary.h"

namespace {
	// The colors of the collision image of a tileset : white and light gray can be walked on, light and dark gray are animated
	constexpr uint8_t White = 0xFF;
	constexpr uint8_t LightGray = 0xAA;
	constexpr uint8_t DarkGray = 0x46;

	bool Gray(const ska::Color& color, uint8_t value) {
		return color.r == value && color.g == value && color.b == value;
	}

	bool Walkable(const ska::Color& color) {
		return Gray(color, White) || Gray(color, LightGray);
	}
}

ska::SDLSurface LoadTilesetImage(const std::string& tilesetName) {
	ska::SDLSurface result;
	result.load32(tilesetName + ".png");
	if (result.getInstance() == nullptr) {
		ska::ExceptionTrigger<ska::FileException>("Erreur lors de l'ouverture du fichier \"" + tilesetName + ".png\", fichier du tileset. " + std::string(ska::SDLLibrary::get().getError()));
	}
	return result;
}

ska::SDLSurface LoadTilesetImageCollisions(const std::string& tilesetName) {
	ska::SDLSurface result;
	result.load32(tilesetName + ".png.col");
	if (result.getInstance() == nullptr) {
		ska::ExceptionTrigger<ska::FileException>("Erreur lors de l'ouverture du fichier \"" + tilesetName + ".png.col\", fichier de collsions du tileset. " + std::string(ska::SDLLibrary::get().getError()));
	}
	return result;
}

ska::SDLSurface LoadTilesetImageProperties(const std::string& tilesetName) {
	ska::SDLSurface result;
	result.load32(tilesetName + ".png.prop");
	if (result.getInstance() == nullptr) {
		ska::ExceptionTrigger<ska::FileException>("Erreur lors de l'ouverture du fichier \"" + tilesetName + ".png.prop\", fichier de proprietes du tileset. " + std::string(ska::SDLLibrary::get().getError()));
	}
	return result;
}


void ska::TilesetLoaderImage::checkSizes() const {
	if(!AllEquals(m_sCol.getInstance()->w, m_sProperties.getInstance()->w) ||
	   !AllEquals(m_sCol.getInstance()->h, m_sProperties.getInstance()->h)) {
		ExceptionTrigger<FileException>("Unable to load the tileset " + m_tilesetName + " from image loader : image files don't have the same dimensions");
	}
}

ska::TilesetLoaderImage::TilesetLoaderImage(std::string tilesetName) :
	m_tilesetName(std::move(tilesetName)),
	m_sChipset(LoadTilesetImage(m_tilesetName)),
	m_sCol(LoadTilesetImageCollisions(m_tilesetName)),
	m_sProperties(LoadTilesetImageProperties(m_tilesetName)) {
	checkSizes();
}

ska::Vector2<ska::Tile> ska::TilesetLoaderImage::loadPhysics() const {
	const auto width = m_sCol.getInstance()->w;
	const auto height = m_sCol.getInstance()->h;

	auto tiles = Vector2<Tile>{};
	tiles.resize(width, height);
	for (auto x = 0; x < width; x++) {
		for (auto y = 0; y < height; y++) {
			const auto col = Walkable(m_sCol.getPixel32Color(x, y)) ? TileCollision::No : TileCollision::Yes;

			auto tileProperties = TileProperties{};
			// The properties image : its red, the bits of the properties of the game (TileProperties::bitMask) ; its
			// green, 0 a flat tile, n + 1 a tile standing with its base n blocks lower
			const auto property = m_sProperties.getPixel32Color(x, y);
			tileProperties.bitMask = property.r;
			tileProperties.base = static_cast<int>(property.g) - 1;

			//TODO propriétés (exemple : vent) ?
			//m_sProperties.getPixel32Color(x, y).r ==

			tiles[x][y] = Tile{ { x, y }, tileProperties, col };
		}
	}
	return tiles;
}

ska::Texture ska::TilesetLoaderImage::loadGraphics() const {
	return Texture { m_tilesetName + ".png"};
}

ska::Vector2<ska::TileAnimation> ska::TilesetLoaderImage::loadAnimations(unsigned int blockSize) const {
	const auto width = m_sCol.getInstance()->w;
	const auto height = m_sCol.getInstance()->h;

	auto tiles = ska::Vector2<TileAnimation>{};
	tiles.resize(width, height);
	for (auto x = 0; x < width; x++) {
		for (auto y = 0; y < height; y++) {
			const auto color = m_sCol.getPixel32Color(x, y);
			const auto autoAnim = Gray(color, DarkGray) || Gray(color, LightGray);
			tiles[x][y].animated = autoAnim;
			
			const auto r = ska::Rectangle{
				static_cast<int>(x * blockSize),
				static_cast<int>(y * blockSize),
				static_cast<int>(blockSize),
				static_cast<int>(blockSize)
			};

			tiles[x][y].animation = autoAnim ? Animation{ 375, 4, true, r.x, r.y, static_cast<unsigned int>(r.w), static_cast<unsigned int>(r.h) } : 
				Animation{ std::numeric_limits<unsigned int>::max(), 1, true, r.x, r.y, static_cast<unsigned int>(r.w), static_cast<unsigned int>(r.h) };
		}
	}

	return tiles;
}

std::string ska::TilesetLoaderImage::getName() const {
	return m_tilesetName;
}
