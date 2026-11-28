#include "World/__internalConfig/LoggerConfig.h"
#include "TileWorld.h"
#include "LayerMonoRenderable.h"
#include "Core/Draw/Renderer.h"
#include "Core/Draw/DrawableContainer.h"

ska::LayerMonoRenderable::LayerMonoRenderable(Vector2<TileAnimation*> block, const Texture& tileset, const unsigned int blockSize, const Point<int> origin) :
	m_tileSize(blockSize),
	m_origin(origin),
	m_animations(std::move(block)),
	m_tileset(tileset),
	m_width(m_animations.lineSize() * m_tileSize),
	m_height(m_width == 0 ? 0 : (m_animations.size() / m_animations.lineSize()) * m_tileSize ) {
}

void ska::LayerMonoRenderable::graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) {
	m_lastCameraPos = cameraPos;
	drawables.add(*this);
}

void ska::LayerMonoRenderable::render(Renderer& renderer) const {
	// The camera from the corner of the layer (negative : the layer starts further right or down on the screen)
	const auto size = static_cast<int>(m_tileSize);
	const auto cameraX = static_cast<int>(NumberUtils::absolute(m_lastCameraPos.x)) - m_origin.x;
	const auto cameraY = static_cast<int>(NumberUtils::absolute(m_lastCameraPos.y)) - m_origin.y;
	if (size == 0 || cameraX + m_lastCameraPos.w < 0 || cameraY + m_lastCameraPos.h < 0) {
		return;
	}
	const auto blocksX = static_cast<int>(m_width) / size;
	const auto blocksY = static_cast<int>(m_height) / size;
	const auto startX = std::max(0, cameraX) / size;
	const auto startY = std::max(0, cameraY) / size;
	const auto endX = std::min(blocksX - 1, (cameraX + m_lastCameraPos.w) / size);
	const auto endY = std::min(blocksY - 1, (cameraY + m_lastCameraPos.h) / size);

	for (auto i = startX; i <= endX; i++) {
		for (auto j = startY; j <= endY; j++) {
			const auto& a = m_animations[i][j];
			if (a != nullptr) {
				auto* currentClip = a->animated ? &a->animation.getCurrentFrame() : &a->animation.getOffsetBase();
				renderer.render(m_tileset, i * size - cameraX, j * size - cameraY, currentClip, 0, nullptr);
			}
		}
	}
}
