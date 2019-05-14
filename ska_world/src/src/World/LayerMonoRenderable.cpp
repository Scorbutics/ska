#include "World/__internalConfig/LoggerConfig.h"
#include "TileWorld.h"
#include "LayerMonoRenderable.h"
#include "Core/Draw/Renderer.h"
#include "Core/Draw/DrawableContainer.h"

ska::LayerMonoRenderable::LayerMonoRenderable(Vector2<TileAnimation*> block, const Texture& tileset, const unsigned int blockSize) :
	m_tileSize(blockSize),
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
	const auto absORelX = NumberUtils::absolute(m_lastCameraPos.x);
	const auto absORelY = NumberUtils::absolute(m_lastCameraPos.y);
	const auto cameraPositionStartBlockX = absORelX / m_tileSize;
	const auto cameraPositionStartBlockY = absORelY / m_tileSize;
	const auto cameraPositionEndBlockX = (absORelX + m_lastCameraPos.w) / m_tileSize;
	const auto cameraPositionEndBlockY = (absORelY + m_lastCameraPos.h) / m_tileSize;

	const auto layerPixelsX = m_width;
	const auto layerPixelsY = m_height;

	for (auto i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (auto j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {
			const auto currentXBlock = i * m_tileSize;
			const auto currentYBlock = j * m_tileSize;
			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				const auto& a = m_animations[i][j];
				if (a != nullptr) {
					auto* currentClip = a->animated ? &a->animation.getCurrentFrame() : &a->animation.getOffsetBase();
					renderer.render(m_tileset, currentXBlock - absORelX, currentYBlock - absORelY, currentClip, 0, nullptr);
				}
			}
		}
	}
}
