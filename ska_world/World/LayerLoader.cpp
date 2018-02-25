#include "LayerLoader.h"
#include "Chipset.h"
#include "Exceptions/FileException.h"
#include "Utils/Vector2.h"

void ska::LayerLoader::load(Vector2<Block*>& layer, const std::string& layerFilename, Chipset& chipset) {
	SDLSurface file;
	file.load32(layerFilename);
	if (file.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + layerFilename + "\"" + std::string(SDL_GetError()));
	}

	const auto fileHeight = static_cast<unsigned int>(file.getInstance()->h);
	const auto fileWidth = static_cast<unsigned int>(file.getInstance()->w);
	const auto area = fileHeight * fileWidth;

	layer.clear();
	layer.reserve(area);
	layer.setLineSize(fileWidth);

	for (auto x = 0; x < fileWidth; x++) {
		for (auto y = 0; y < fileHeight; y++) {
			const auto color = file.getPixel32Color(x, y);
			Block* block;
			BlockRenderable* br;
			std::tie(block, br) = chipset.getBlock(color);
			layer.push_back(block);
		}
	}
}

