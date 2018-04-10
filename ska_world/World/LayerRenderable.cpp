#include "Exceptions/IndexOutOfBoundsException.h"
#include "Utils/StringUtils.h"

#include "TileWorld.h"
#include "LayerRenderable.h"
#include "Draw/Renderer.h"

ska::LayerRenderable::LayerRenderable(Vector2<BlockRenderable*>&& block, ChipsetRenderable& chipset, const unsigned int blockSize) :
	m_blockSize(blockSize),
	m_chipset(chipset),
	m_block(std::move(block)),
	m_width(m_block.lineSize()),
	m_height(m_width == 0 ? 0 : m_block.size() / m_width) {
	m_block.reserve(m_width, m_height);
}

void ska::LayerRenderable::update(const ska::Rectangle& cameraPos) {
	m_lastCameraPos = cameraPos;

	/* TODO external view ? (avoid camera reference)*/
	const auto absORelX = NumberUtils::absolute(cameraPos.x);
	const auto absORelY = NumberUtils::absolute(cameraPos.y);
	const auto cameraPositionStartBlockX = absORelX / m_blockSize;
	const auto cameraPositionStartBlockY = absORelY / m_blockSize;
	const auto cameraPositionEndBlockX = (absORelX + cameraPos.w) / m_blockSize;
	const auto cameraPositionEndBlockY = (absORelY + cameraPos.h) / m_blockSize;

	const auto layerPixelsX = m_width;
	const auto layerPixelsY = m_height;

	for (auto i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (auto j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {
			const auto currentXBlock = i * m_blockSize;
			const auto currentYBlock = j * m_blockSize;

			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				const auto b = m_block[i][j];
				if (b != nullptr) {
					m_chipset.update(*b);
				}
			}

		}
	}
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
				const auto b = m_block[i][j];
				if (b != nullptr) {
					const ska::Point<int> absoluteCurrentPos(currentXBlock - absORelX, currentYBlock - absORelY);
					/* TODO passer la propriété BLOCK_PROP_WIND_SENSITIVITY en script de chipset */
					m_chipset.render(renderer, absoluteCurrentPos, *b);
				}
			}
		}
	}
}

ska::BlockRenderable* ska::LayerRenderable::getBlock(const unsigned int i, const unsigned int j) {
	if (i < m_block.size() && j < m_block[i].size()) {
		return m_block[i][j];
	}
	
	throw IndexOutOfBoundsException("block at coordinates (" + StringUtils::intToStr(i) + "; " + StringUtils::intToStr(j) + ") cannot be accessed");
}

