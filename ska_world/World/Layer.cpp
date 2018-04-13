#include "Layer.h"
#include "TileWorld.h"

ska::Layer::Layer(ska::Vector2<ska::Tile*>&& block) :
	m_fileWidth(0),
	m_fileHeight(0) {
	reset(std::move(block));
}

unsigned ska::Layer::getBlocksX() const {
	return m_fileWidth;
}

unsigned ska::Layer::getBlocksY() const {
	return m_fileHeight;
}

ska::Tile const* ska::Layer::getBlock(const unsigned x, const unsigned y) const {
	return m_block[x][y];
}

ska::TileCollision ska::Layer::getCollision(const unsigned int x, const unsigned int y) const {
	if (m_block.has(x, y)) {
		const auto* b = m_block[x][y];
		if (b == nullptr) {
			return TileCollision::Void;
		}
		return b->collision;
	}
	return ska::TileCollision::Yes;
}

void ska::Layer::reset(ska::Vector2<ska::Tile*>&& block) {
	m_block = std::move(block);
	m_fileWidth = m_block.lineSize();
	m_fileHeight = m_fileWidth == 0 ? 0 : m_block.size() / m_fileWidth;
}
