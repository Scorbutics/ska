#include "Tile.h"
#include "Tileset.h"
#include "Graphic/Animation.h"

ska::Tileset::Tileset(const unsigned int tileSize, const TilesetLoader& loader, const TilesetEventLoader& loaderEvent) :
	m_tilesetName(loader.getName()),
	m_tileSize(tileSize),
	m_renderable(tileSize, loader),
	m_event(m_tilesetName, loaderEvent),
	m_blocks(loader.loadPhysics()){
}

const ska::Texture& ska::Tileset::getTexture() const {
	return m_renderable.getTexture();
}

const std::string& ska::Tileset::getName() const {
	return m_tilesetName;
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

const ska::TilesetEvent& ska::Tileset::getTilesetEvent() const {
	return m_event;
}

ska::Tile& ska::Tileset::getTile(const Point<int>& coordinates) {
	return m_blocks[coordinates.x][coordinates.y];
}

void ska::Tileset::update() {
	m_renderable.update();
}

ska::TileAnimation& ska::Tileset::getAnimation(const Point<int>& coordinates) {
	return m_renderable.getAnimation(coordinates);
}
