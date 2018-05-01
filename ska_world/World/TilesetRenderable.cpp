#include "Draw/Renderer.h"
#include "TilesetRenderable.h"
#include "TilesetLoader.h"

ska::TilesetRenderable::TilesetRenderable(const unsigned int tileSize, const TilesetLoader& loader) :
	m_tilesetName(loader.getName()),
	m_tileSize(tileSize) {
	load(loader);
}

const ska::Texture& ska::TilesetRenderable::getTexture() const{
	return m_tileset;
}

ska::TileAnimation& ska::TilesetRenderable::getAnimation(const Point<int>& pos) {
	return m_animations[pos.x][pos.y];
}

void ska::TilesetRenderable::update() {
	for (auto& anim : m_animations) {
		if (anim.animated) {
			anim.animation.updateFrame();
		}
	}
}

void ska::TilesetRenderable::load(const TilesetLoader& loader) {
    m_tileset = loader.loadGraphics();
	m_animations = loader.loadAnimations(m_tileSize);
}
