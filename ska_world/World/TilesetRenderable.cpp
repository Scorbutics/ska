#include "../World/TileRenderable.h"
#include "Draw/Renderer.h"
#include "TilesetRenderable.h"
#include "TilesetLoader.h"

ska::TilesetRenderable::TilesetRenderable(const unsigned int tileSize, const TilesetLoader& loader) :
	m_tilesetName(loader.getName()),
	m_tileSize(tileSize) {
	load(loader);
}

void ska::TilesetRenderable::render(const Renderer& renderer, const Point<int> pos, const TileRenderable& block) const {
	block.refresh();
	auto chipsetPartRender = block.determineFrame(nullptr);
	renderer.render(m_tileset, pos.x, pos.y, &chipsetPartRender);
}

const ska::TileRenderable* ska::TilesetRenderable::getTile(const Point<int> posCorr) const {
	return m_blocks.has(posCorr.x, posCorr.y) ? nullptr : &m_blocks[posCorr.x][posCorr.y];
}

const ska::Texture& ska::TilesetRenderable::getTexture() const{
	return m_tileset;
}

void ska::TilesetRenderable::load(const TilesetLoader& loader) {
	std::tie(m_tileset, m_blocks) = loader.loadGraphics(m_tileSize);
}
