#include "Layer.h"
#include "TileWorld.h"
#include <cassert>

ska::Layer::Layer(Vector2<std::optional<Tile>> block, const bool top) :
	m_fileWidth(0),
	m_fileHeight(0),
	m_top(top) {
	reset(std::move(block));
}

unsigned ska::Layer::getBlocksX() const {
	return m_fileWidth;
}

unsigned ska::Layer::getBlocksY() const {
	return m_fileHeight;
}

ska::Tile const* ska::Layer::getBlock(const std::size_t x, const std::size_t y) const {
	if (!m_block.has(x, y)) {
		return nullptr;
	}
	
	return m_block[x][y].has_value() ? &m_block[x][y].value() : nullptr;
}

bool ska::Layer::isTop() const {
	return m_top;
}

ska::TileCollision ska::Layer::getCollision(const std::size_t x, const std::size_t y) const {
	if (m_block.has(x, y)) {
		const auto& b = m_block[x][y];
		if (!b.has_value()) {
			return TileCollision::Void;
		}
		return b.value().collision;
	}
	return TileCollision::Yes;
}

void ska::Layer::reset(Vector2<std::optional<Tile>> block) {
	m_block = std::move(block);
	m_fileWidth = m_block.lineSize();
	m_fileHeight = m_fileWidth == 0 ? 0 : m_block.size() / m_fileWidth;
}
