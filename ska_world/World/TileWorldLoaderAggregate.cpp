#include "TileWorldLoaderAggregate.h"
#include "Layer.h"
#include "LayerLoader.h"
#include "LayerRenderable.h"
#include "TileWorld.h"
#include "LayerEventLoaderText.h"

ska::TileWorldLoaderAggregate::TileWorldLoaderAggregate(
                                                std::string levelPath,
                                                std::vector<std::unique_ptr<LayerLoader>> loaders,
                                                std::vector<std::unique_ptr<LayerEventLoader>> eventLoaders) :
    m_levelPath(std::move(levelPath)),
    m_loaders(std::move(loaders)),
    m_eventLoaders(std::move(eventLoaders)) {
}

ska::CollisionProfile ska::TileWorldLoaderAggregate::loadPhysics(Tileset& tileset) const {
	auto layers = std::vector<LayerPtr> {m_loaders.size()};
	for (const auto& l : m_loaders) {
		layers.emplace_back(std::make_unique<Layer>(l->loadPhysics(tileset)));
	}
	
	return CollisionProfile{ tileset.getTileSize(), std::move(layers) };
}

std::vector<ska::LayerRenderablePtr> ska::TileWorldLoaderAggregate::loadGraphics(Tileset& tileset, const unsigned int blockSize) const {
	std::vector<LayerRenderablePtr> renderables;
	for (const auto& l : m_loaders) {
		renderables.push_back(std::make_unique<LayerRenderable>(l->loadAnimations(tileset), tileset.getTexture(), blockSize));
	}
	return renderables;
}

std::vector<ska::LayerEventPtr> ska::TileWorldLoaderAggregate::loadEvents(unsigned int widthLayer, unsigned int heightLayer) const {
    auto events = std::vector<LayerEventPtr>{};
    for(const auto& l : m_eventLoaders) {
        events.push_back(std::make_unique<LayerEvent>(*l, widthLayer, heightLayer));
    }
    return events;
}

const std::string& ska::TileWorldLoaderAggregate::getName() const {
	return m_levelPath;
}
