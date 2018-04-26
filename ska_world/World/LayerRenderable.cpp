#include "TileWorld.h"
#include "LayerRenderable.h"
#include "Draw/Renderer.h"

ska::LayerRenderable::LayerRenderable(Vector2<Animation*> block, const Texture& tileset, const unsigned int blockSize) :
	m_tileSize(blockSize),
	m_animations(std::move(block)),
	m_tileset(tileset),
	m_width(m_animations.lineSize()),
	m_height(m_width == 0 ? 0 : m_animations.size() / m_width) {
}

void ska::LayerRenderable::update(const ska::Rectangle& cameraPos) {
	m_lastCameraPos = cameraPos;
}

bool ska::LayerRenderable::isVisible() const {
	return !m_animations.empty();
}

void ska::LayerRenderable::clear() {
	m_animations.clear();
}

void ska::LayerRenderable::render(const Renderer& renderer) const {
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
				auto* currentClip = a != nullptr ? &a->getCurrentFrame() : nullptr;
				renderer.render(m_tileset, currentXBlock - absORelX, currentYBlock - absORelY, currentClip, 0, nullptr);
			}
		}
	}
}
