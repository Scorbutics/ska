#include "TileWorldLoaderImage.h"
#include "Layer.h"
#include "LayerLoader.h"
#include "LayerRenderable.h"
#include "TileWorld.h"
#include "LayerEventLoaderText.h"

ska::TileWorldLoaderImage::TileWorldLoaderImage(
                                                std::string levelPath,
                                                std::vector<std::unique_ptr<LayerLoader>> loaders,
                                                std::vector<std::unique_ptr<LayerEventLoader>> eventLoaders) :
    m_levelPath(std::move(levelPath)),
    m_loaders(std::move(loaders)),
    m_eventLoaders(std::move(eventLoaders)) {
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

std::vector<ska::LayerEventPtr> ska::TileWorldLoaderImage::loadEvents() const {
    auto events = std::vector<LayerEventPtr>{};
    for(const auto& l : m_eventLoaders) {
        events.push_back(std::make_unique<LayerEvent>(m_levelPath, l->load()));
    }
    return events;
}

const std::string& ska::TileWorldLoaderImage::getName() const {
	return m_levelPath;
}
