#include "Exceptions/IndexOutOfBoundsException.h"
#include "Utils/StringUtils.h"

#include "TileWorld.h"
#include "LayerRenderable.h"
#include "Draw/Renderer.h"

ska::LayerRenderable::LayerRenderable(Vector2<std::optional<const TileRenderable>> block, const TilesetRenderable& chipset, const unsigned int blockSize) :
	m_blockSize(blockSize),
	m_tileset(chipset),
	m_lastCameraPos(),
	m_block(std::move(block)),
	m_width(m_block.lineSize()),
	m_height(m_width == 0 ? 0 : m_block.size() / m_width) {
}

void ska::LayerRenderable::update(const ska::Rectangle& cameraPos) {
	m_lastCameraPos = cameraPos;
}

bool ska::LayerRenderable::isVisible() const {
	return !m_block.empty();
}

void ska::LayerRenderable::clear() {
	m_block.clear();
}

void ska::LayerRenderable::render(const Renderer& renderer) const {

	const auto absORelX = NumberUtils::absolute(m_lastCameraPos.x);
	const auto absORelY = NumberUtils::absolute(m_lastCameraPos.y);
	const auto cameraPositionStartBlockX = absORelX / m_blockSize;
	const auto cameraPositionStartBlockY = absORelY / m_blockSize;
	const auto cameraPositionEndBlockX = (absORelX + m_lastCameraPos.w) / m_blockSize;
	const auto cameraPositionEndBlockY = (absORelY + m_lastCameraPos.h) / m_blockSize;

	const auto layerPixelsX = m_width;
	const auto layerPixelsY = m_height;

	for (auto i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (auto j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {
			const auto currentXBlock = i * m_blockSize;
			const auto currentYBlock = j * m_blockSize;
			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				const auto& b = m_block[i][j];
				if (b.has_value()) {
					const ska::Point<int> absoluteCurrentPos(currentXBlock - absORelX, currentYBlock - absORelY);
					m_tileset.render(renderer, absoluteCurrentPos, b.value());
				}
			}
		}
	}
}

const ska::TileRenderable* ska::LayerRenderable::getBlock(const unsigned int i, const unsigned int j) {
	if (m_block.has(i, j)) {
		return m_block[i][j].has_value() ? &m_block[i][j].value() : nullptr;
	}

	throw IndexOutOfBoundsException("block at coordinates (" + StringUtils::intToStr(i) + "; " + StringUtils::intToStr(j) + ") cannot be accessed");
}

