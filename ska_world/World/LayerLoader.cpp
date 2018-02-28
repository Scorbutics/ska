#include "LayerLoader.h"
#include "Chipset.h"
#include "Exceptions/FileException.h"
#include "Utils/Vector2.h"
#include "LayerData.h"

ska::LayerData ska::LayerLoader::load(const std::string& layerFilename, Chipset& chipset) {
	SDLSurface file;
	file.load32(layerFilename);
	if (file.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + layerFilename + "\"" + std::string(SDL_GetError()));
	}

	const auto fileHeight = static_cast<unsigned int>(file.getInstance()->h);
	const auto fileWidth = static_cast<unsigned int>(file.getInstance()->w);
	const auto area = fileHeight * fileWidth;
	
	LayerData layerData;
	layerData.physics.reserve(area);
	layerData.graphics.reserve(area);
	layerData.physics.setLineSize(fileWidth);
	layerData.graphics.setLineSize(fileWidth);

	for (auto y = 0; y < fileHeight; y++) {
		for (auto x = 0; x < fileWidth; x++) {
			const auto color = file.getPixel32Color(x, y);
			Block* block;
			BlockRenderable* br;
			std::tie(block, br) = chipset.getBlock(color);
			layerData.physics.push_back(block);
			layerData.graphics.push_back(br);
		}
	}
	return layerData;
}

