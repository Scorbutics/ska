#include "LayerRenderableRow.h"
#include "Draw/DrawableContainer.h"
#include "TileAnimation.h"
#include "Draw/Renderer.h"

void ska::LayerRenderableRow::load(std::size_t rowIndex, Vector2<TileAnimation*>& blocks, const Texture & tileset, unsigned int blockSize) {
	m_tileset = &tileset;
	m_blockSize = blockSize;
	m_width = blockSize * blocks.lineSize();
	m_offset = rowIndex * blockSize;
	m_rowIndex = rowIndex;
	m_animations.resize(blocks.lineSize());

	auto row = blocks[rowIndex];
	for (auto i = 0u; i < row.lineSize; i++) {
		m_animations[i] = row[i];
	}
	Priorized::setPriority(rowIndex * blockSize);
}

void ska::LayerRenderableRow::graphicUpdate(const Rectangle & cameraPos, DrawableContainer& drawables) {
	m_lastCameraPos = cameraPos;
	drawables.add(*this);
}

void ska::LayerRenderableRow::render(const Renderer & renderer) const {
	const auto absORelX = NumberUtils::absolute(m_lastCameraPos.x);
	const auto absORelY = NumberUtils::absolute(m_lastCameraPos.y);
	const auto cameraPositionStartBlockX = absORelX / m_blockSize;
	const auto cameraPositionEndBlockX = (absORelX + m_lastCameraPos.w) / m_blockSize;

	if (absORelY > (m_offset + m_blockSize) || (absORelY + m_lastCameraPos.h) < m_offset) {
		return;
	}

	for (auto i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		const auto currentXBlock = i * m_blockSize;
		if (currentXBlock < m_width) {
			const auto& a = m_animations[i];
			if (a != nullptr) {
				auto* currentClip = a->animated ? &a->animation.getCurrentFrame() : &a->animation.getOffsetBase();
				renderer.render(*m_tileset, currentXBlock - absORelX, m_offset - absORelY, currentClip, 0, nullptr);
			}
		}
	}
}
