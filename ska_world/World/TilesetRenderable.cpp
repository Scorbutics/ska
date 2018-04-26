#include "../World/TileRenderable.h"
#include "Draw/Renderer.h"
#include "TilesetRenderable.h"
#include "TilesetLoader.h"

ska::TilesetRenderable::TilesetRenderable(const unsigned int tileSize, const TilesetLoader& loader) :
	m_tilesetName(loader.getName()),
	m_tileSize(tileSize) {
	load(loader);
}

void ska::TilesetRenderable::render(const Renderer& renderer, const Point<int> pos, const Point<int>& blockId) const {
	if(m_animations.has(blockId.x, blockId.y)) {
        const auto& anim = m_animations[blockId.x][blockId.y];
        auto* chipsetPartRender = anim.has_value() ? &anim.value() : nullptr;
        renderer.render(m_tileset, pos.x, pos.y, chipsetPartRender);
	}
}

const ska::Texture& ska::TilesetRenderable::getTexture() const{
	return m_tileset;
}

void ska::TilesetRenderable::load(const TilesetLoader& loader) {
    m_tileset = loader.loadTexture();
	m_animations = loader.loadAnimations();
}
