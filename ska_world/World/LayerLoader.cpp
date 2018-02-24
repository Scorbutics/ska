#include "LayerLoader.h"
#include "Chipset.h"
#include "Exceptions/FileException.h"

void ska::LayerLoader::load(std::vector<Block*>& layer, const std::string& layerFilename, Chipset& chipset) {
	m_file.load32(layerFilename);
	if (m_file.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + layerFilename + "\"" + std::string(SDL_GetError()));
	}

	const auto fileHeight = static_cast<unsigned int>(m_file.getInstance()->h);
	const auto fileWidth = static_cast<unsigned int>(m_file.getInstance()->w);
	const auto area = fileHeight * fileWidth;

	layer.clear();
	layer.reserve(area);

	for (auto i = 0U; i < area; i++) {
		const auto color = Color(m_file.getPixel32(i));
		Block* block;
		BlockRenderable* br;
		std::tie(block, br) = chipset.generateBlock(color);
		layer[i] = block;
	}
}

