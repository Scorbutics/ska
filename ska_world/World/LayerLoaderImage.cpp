#include <SDL.h>
#include "LayerLoaderImage.h"
#include "Exceptions/FileException.h"
#include "Utils/Vector2.h"
#include "Utils/StringUtils.h"
#include "Graphic/SDLSurface.h"
#include "Tileset.h"
#include "Exceptions/CorruptedFileException.h"

ska::Vector2<ska::Animation*> ska::LayerLoaderImage::loadAnimations(Tileset& chipset) const {
	auto graphics = ska::Vector2<Animation*>{};
	graphics.reserve(m_fileWidth, m_fileHeight);

	const auto& map = m_colorMapper.access();

	for (auto y = 0; y < m_fileHeight; y++) {
		for (auto x = 0; x < m_fileWidth; x++) {
			const auto& color = m_file.getPixel32Color(x, y);

			if(color.r == 255 && color.g == 255 && color.b == 255) {
				graphics.push_back(nullptr);
			} else {
				if (map.find(color) == map.end()) {
                    const auto& ss = StringUtils::intToStr(color.r) + "; " + StringUtils::intToStr(color.g) + "; " + StringUtils::intToStr(color.b);
					throw CorruptedFileException("Impossible de trouver la correspondance en pixel de " + ss + " (fichier niveau corrompu)");
				}
				auto& anim = chipset.getAnimation(map.at(color));
				graphics.push_back(anim.has_value() ? &anim.value() : nullptr);
			}
		}
	}
	return graphics;
}

ska::Vector2<std::optional<ska::Tile>> ska::LayerLoaderImage::loadPhysics(Tileset& chipset) const {
	auto physics = ska::Vector2<std::optional<Tile>> {};
	physics.reserve(m_fileWidth, m_fileHeight);

	const auto& map = m_colorMapper.access();

	for (auto y = 0; y < m_fileHeight; y++) {
		for (auto x = 0; x < m_fileWidth; x++) {
			const auto& color = m_file.getPixel32Color(x, y);

			if (color.r == 255 && color.g == 255 && color.b == 255) {
				physics.push_back(std::optional<Tile>{});
			} else {
				if (map.find(color) == map.end()) {
                    const auto ss = StringUtils::intToStr(color.r) + "; " + StringUtils::intToStr(color.g) + "; " + StringUtils::intToStr(color.b);
					throw CorruptedFileException("Impossible de trouver la correspondance en pixel de " + ss + " (fichier niveau corrompu)");
				}
				physics.push_back(chipset.getTile(map.at(color)));
			}
		}
	}
	return physics;
}

ska::SDLSurface ska::LayerLoaderImage::loadFrom32(const std::string& layerFilename) {
	SDLSurface file;
	file.load32(layerFilename);
	if (file.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + layerFilename + "\"" + std::string(SDL_GetError()));
	}
	return file;
}

ska::LayerLoaderImage::LayerLoaderImage(const TilesetCorrespondanceMapper& correspondanceMapper, const std::string & layerFilename) :
	m_colorMapper(correspondanceMapper),
	m_file(loadFrom32(layerFilename)),
	m_fileWidth(m_file.getInstance()->w),
	m_fileHeight(m_file.getInstance()->h) {
}

