#include "Draw/Renderer.h"
#include "TilesetRenderable.h"
#include "TilesetLoader.h"

ska::TilesetRenderable::TilesetRenderable(const unsigned int tileSize, const TilesetLoader& loader) :
	m_tilesetName(loader.getName()),
	m_tileSize(tileSize) {
	load(loader);
}

void ska::TilesetRenderable::render(const Renderer& renderer, const Point<int>& pos, const Point<int>& tileId) const {
	if(m_animations.has(tileId.x, tileId.y)) {
        auto& anim = m_animations[tileId.x][tileId.y];
        auto* chipsetPartRender = anim.has_value() ? &anim.value().updateFrame() : nullptr;
        renderer.render(m_tileset, pos.x, pos.y, chipsetPartRender);
	}
}

const ska::Texture& ska::TilesetRenderable::getTexture() const{
	return m_tileset;
}

std::optional<ska::Animation>& ska::TilesetRenderable::getAnimation(const Point<int>& pos) {
	return m_animations[pos.x][pos.y];
}

void ska::TilesetRenderable::load(const TilesetLoader& loader) {
    m_tileset = loader.loadGraphics();
	m_animations = loader.loadAnimations(m_tileSize);
}
