#include "TileWorldLoaderImage.h"
#include "Layer.h"
#include "LayerLoader.h"
#include "LayerRenderable.h"
#include "TileWorld.h"

ska::TileWorldLoaderImage::TileWorldLoaderImage(const std::string& corrFileName, std::string levelPath) :
	m_correspondanceMapper(corrFileName),
	m_levelPath(std::move(levelPath)) {
	m_loaders.emplace_back(std::make_unique<LayerLoaderImage>(m_correspondanceMapper, m_levelPath + ".bmp"));
	m_loaders.emplace_back(std::make_unique<LayerLoaderImage>(m_correspondanceMapper, m_levelPath + "M.bmp"));
	m_loaders.emplace_back(std::make_unique<LayerLoaderImage>(m_correspondanceMapper, m_levelPath + "T.bmp"));
}

ska::CollisionProfile ska::TileWorldLoaderImage::loadPhysics(Tileset& tileset) const {
	CollisionProfile profile;
	for (const auto& l : m_loaders) {
		profile.addLayer(std::make_unique<Layer>(l->loadPhysics(tileset)));
	}
	return profile;
}

std::vector<ska::LayerRenderablePtr> ska::TileWorldLoaderImage::loadGraphics(Tileset& tileset, const unsigned int blockSize) const {
	std::vector<LayerRenderablePtr> renderables;
	for (const auto& l : m_loaders) {
		renderables.push_back(std::make_unique<LayerRenderable>(l->loadGraphics(tileset), tileset.getRenderable(), blockSize));
	}
	return renderables;
}

const std::string& ska::TileWorldLoaderImage::getName() const {
	return m_levelPath;
}
