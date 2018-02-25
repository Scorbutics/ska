#include <string>

#include "Layer.h"
#include "World.h"
#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/IndexOutOfBoundsException.h"
#include "Utils/StringUtils.h"
#include "Exceptions/FileException.h"
#include "Utils/FileUtils.h"

//Constructeur ouvrant un monde déjà créé
ska::Layer::Layer(World& w, const std::string& pathFile, Layer* parent) : 
	m_world(w), 
	m_renderable(w) {
	m_block.reserve(20);
	m_parent = parent;
	m_fileWidth = 0;
	m_fileHeight = 0;
    reset(pathFile);
}

ska::Layer::Layer(World& w, Layer*) : m_parent(nullptr), m_world(w), m_renderable(w), m_fileWidth(0), m_fileHeight(0){
}

ska::Layer* ska::Layer::getParent() const {
	return m_parent;
}

unsigned ska::Layer::getBlocksX() const {
	return m_fileWidth;
}

unsigned ska::Layer::getBlocksY() const {
	return m_fileHeight;
}

ska::LayerRenderable& ska::Layer::getRenderable() {
	return m_renderable;
}

void ska::Layer::clear() {
	m_block.clear();
}

ska::Block* ska::Layer::getBlock(const unsigned int i, const unsigned int j) const
{
	if (i < m_block.size() && j < m_block[i].size()) {
		return m_block[i][j];
	} else {
		throw IndexOutOfBoundsException("block at coordinates (" + StringUtils::intToStr(i) + "; " + StringUtils::intToStr(j) + ") cannot be accessed");
    }

}

int ska::Layer::getBlockCollision(const unsigned int i, const unsigned int j) const {
	if (i < m_block.size() && j < m_block[i].size()) {
		const auto* b = m_block[i][j];
		if (b == nullptr) {
			return BLOCK_COL_VOID;
		}
		return b->getCollision();
	}
	return BLOCK_COL_YES;
}


void ska::Layer::reset(const std::string& pathFile) {

	const FileNameData fnd(pathFile);
	m_nomFichier = fnd.name + "." + fnd.extension;
    m_name = fnd.name;

    SDLSurface fichierMPng;

    fichierMPng.load32(pathFile);
	if (fichierMPng.getInstance() == nullptr) {
		throw FileException("Erreur lors du chargement de la couche " + m_name + " : " + std::string(SDL_GetError()));
	}

	m_fileHeight = static_cast<unsigned int>(fichierMPng.getInstance()->h);
	m_fileWidth = static_cast<unsigned int>(fichierMPng.getInstance()->w);

	/* Layer coherence check */
	checkSize(m_fileWidth, m_fileHeight);

	const auto chipset = m_world.getChipset();
	if (chipset != nullptr) {
		Vector2<BlockRenderable*> renderableBlocks;
		m_block.reserve(m_fileWidth * m_fileHeight);
		renderableBlocks.resize(m_fileWidth * m_fileHeight);

		for (auto i = 0U; i < m_fileWidth; i++) {
			renderableBlocks.reserve(m_fileHeight);
			for (auto j = 0U; j < m_fileHeight; j++) {
				auto c = fichierMPng.getPixel32Color(i, j);
				BlockRenderable* brp = nullptr;
				Block* bp = nullptr;
				std::tie(bp, brp) = chipset->getBlock(c);
				m_block.push_back(bp);
				renderableBlocks.push_back(brp);
			}
		}

		m_renderable.reset(std::move(renderableBlocks));
	}

}

void ska::Layer::checkSize(unsigned int nbrBlocX, unsigned int nbrBlocY) const{
	if (m_fileWidth != nbrBlocX || m_fileHeight != nbrBlocY) {
		throw CorruptedFileException("Layer " + m_name + " has a wrong size dimension in his file " + m_nomFichier);
	}

	if (m_parent != nullptr) {
		m_parent->checkSize(nbrBlocX, nbrBlocY);
	}
}



