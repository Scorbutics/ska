#include <utility>
#include "../World/BlockRenderable.h"
#include "Utils/StringUtils.h"
#include "ChipsetRenderable.h"
#include "Draw/Renderer.h"


ska::ChipsetRenderable::ChipsetRenderable(const unsigned int corrFileSize, const int blockSize, const std::string& chipsetName) :
m_animBlocks(375, 4, true, 0, 0, blockSize, blockSize) {
	load(chipsetName);
	m_blocks.resize(corrFileSize);
}

void ska::ChipsetRenderable::render(const Renderer& renderer, Point<int> pos, const BlockRenderable& block) const{
	auto clip = m_animBlocks.getCurrentFrame();
	auto chipsetPartRender = block.determineFrame(pos, &clip);
	renderer.render(m_chipset, pos.x, pos.y, &chipsetPartRender);
}

void ska::ChipsetRenderable::update(BlockRenderable& block) {
	block.refresh();
	m_animBlocks.updateFrame();
}

ska::BlockRenderable& ska::ChipsetRenderable::getBlock(const int id, const int blockSize, Point<int> posCorr, bool auto_anim) const {
	if (m_blocks[id] == nullptr) {
		m_blocks[id] = std::move(std::make_unique<BlockRenderable>(blockSize, posCorr, auto_anim));
	}
	return *m_blocks[id].get();
}

void ska::ChipsetRenderable::load(const std::string& chipsetName) {
	m_chipset.load(chipsetName);
}


const ska::Texture& ska::ChipsetRenderable::getTexture() const{
	return m_chipset;
}
