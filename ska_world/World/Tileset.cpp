#include "../World/TileRenderable.h"
#include "Physic/Tile.h"
#include "Tileset.h"

ska::Tileset::Tileset(const unsigned int tileSize, const TilesetLoader& loader, const TilesetEventLoader& loaderEvent) :
	m_tilesetName(loader.getName()),
	m_tileSize(tileSize),
	m_renderable(tileSize, loader),
	m_event(m_tilesetName, loaderEvent),
	m_blocks(loader.loadPhysics()){
}

const std::string& ska::Tileset::getName() const {
	return m_tilesetName;
}

const ska::TilesetRenderable& ska::Tileset::getRenderable() const {
	return m_renderable;
}

unsigned int ska::Tileset::getWidth() const {
	return m_renderable.getTexture().getWidth();
}

unsigned int ska::Tileset::getHeight() const {
	return m_renderable.getTexture().getHeight();
}

unsigned ska::Tileset::getTileSize() const {
	return m_tileSize;
}

std::pair<ska::Tile*, const ska::TileRenderable*> ska::Tileset::getTile(const Point<int>& coordinates) {
	if(!m_blocks.has(coordinates.x, coordinates.y)) {
		return std::make_pair(nullptr, nullptr);
	}

	return std::make_pair(&m_blocks[coordinates.x][coordinates.y], m_renderable.getTile(coordinates));
}
