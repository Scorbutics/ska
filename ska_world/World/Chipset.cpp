#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/FileException.h"
#include "../World/Block.h"
#include "../World/BlockRenderable.h"
#include "Chipset.h"

ska::Chipset::Chipset(const int blockSize, const std::string& chipsetName) :
	m_chipsetCorrespondanceMapper(chipsetName),
	m_blockSize(blockSize),
	m_chipsetName(chipsetName),
	m_renderable(static_cast<const unsigned int>(m_chipsetCorrespondanceMapper.access().size()), blockSize, chipsetName) {
	load();
	m_blocks.resize(m_chipsetCorrespondanceMapper.access().size());
}

void ska::Chipset::load() {
	m_sChipset.load32(m_chipsetName);
	if (m_sChipset.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + m_chipsetName + "\", fichier du chipset. " + std::string(SDL_GetError()));
	}

	m_sCol.load32(m_chipsetName + ".col");
	if (m_sCol.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + m_chipsetName + ".col\", fichier de collsions du chipset. " + std::string(SDL_GetError()));
	}

	m_sProperties.load32(m_chipsetName + ".prop");
	if (m_sProperties.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + m_chipsetName + ".prop\", fichier de propri�t�s du chipset. " + std::string(SDL_GetError()));
	}
}

const std::string& ska::Chipset::getName() const {
	return m_chipsetName;
}

ska::ChipsetRenderable& ska::Chipset::getRenderable() {
	return m_renderable;
}

//TODO charger totalit� du chipset
std::pair<ska::Block*, ska::BlockRenderable*> ska::Chipset::generateBlock(const Color& key) {
	Block* outputBlock = nullptr; 
	BlockRenderable* outputRenderable = nullptr;

	if (key.r != 255 || key.g != 255 || key.b != 255) {
		const auto& map = m_chipsetCorrespondanceMapper.access();
		if (map.find(key) != map.end()) {
			const auto& posCorr = map.at(key);
			const auto prop = m_sProperties.getPixel32Color(posCorr.x, posCorr.y).r;
			const auto col = m_sCol.getPixel32(posCorr.x, posCorr.y);

			const auto collision = (col == m_whiteColor || col == m_lightColor) ? BLOCK_COL_NO : BLOCK_COL_YES;
			const auto autoAnim = (col == m_darkColor || col == m_lightColor);

			const auto corrFileWidth = m_chipsetCorrespondanceMapper.getFileWidth();
			const int id = posCorr.x + posCorr.y * corrFileWidth;
			if (m_blocks[id] == nullptr) {
				m_blocks[id] = std::move(std::make_unique<Block>(corrFileWidth, posCorr, prop, collision));
			}
			
			outputBlock = m_blocks[id].get();
			outputRenderable = m_renderable.generateBlock(id, m_blockSize, posCorr, autoAnim).get();
		} else {
			throw CorruptedFileException("Impossible de trouver la correspondance en pixel (fichier niveau corrompu)");
		}
	}

	return std::make_pair(outputBlock, outputRenderable);
}
