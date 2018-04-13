#include "../World/TileRenderable.h"
#include "Physic/Tile.h"
#include "Tileset.h"
#include "TilesetCorrespondanceMapper.h"
#include "Exceptions/CorruptedFileException.h"

ska::Tileset::Tileset(const unsigned int tileSize, const TilesetLoader& loader) :
	m_tilesetName(loader.getName()),
	m_renderable(tileSize, loader),
	m_tileSize(tileSize) {
	load(loader);
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

void ska::Tileset::load(const TilesetLoader& loader) {
	m_blocks = loader.loadPhysics();
}

std::pair<ska::Tile*, const ska::TileRenderable*> ska::Tileset::getTile(const Point<int>& coordinates) {

	if(!m_blocks.has(coordinates.x, coordinates.y)) {
		return std::make_pair(nullptr, nullptr);
	}

	auto& outputBlock = m_blocks[coordinates.x][coordinates.y];
	const auto& outputRenderable = m_renderable.getTile(coordinates);
	
	return std::make_pair(&outputBlock, &outputRenderable);

}
