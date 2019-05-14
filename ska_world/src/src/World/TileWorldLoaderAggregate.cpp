#include "World/__internalConfig/LoggerConfig.h"
#include "TileWorldLoaderAggregate.h"
#include "Layer.h"
#include "LayerLoader.h"
#include "LayerRenderable.h"
#include "TileWorld.h"
#include "LayerEventLoaderText.h"
#include "LayerYIndexRenderable.h"

ska::TileWorldLoaderAggregate::TileWorldLoaderAggregate(
                                                std::string levelPath,
												const std::size_t layerFixed,
                                                std::vector<std::unique_ptr<LayerLoader>> loaders,
                                                std::vector<std::unique_ptr<LayerEventLoader>> eventLoaders) :
	m_layerFixed(layerFixed > loaders.size() ? loaders.size() : layerFixed),
	m_levelPath(std::move(levelPath)),
    m_loaders(std::move(loaders)),
    m_eventLoaders(std::move(eventLoaders)) {
}

ska::CollisionProfile ska::TileWorldLoaderAggregate::loadPhysics(Tileset& tileset) const {
	auto layers = std::vector<LayerPtr> {};
	layers.reserve(m_loaders.size());
	for (const auto& l : m_loaders) {
		layers.emplace_back(std::make_unique<Layer>(l->loadPhysics(tileset)));
	}
	
	return CollisionProfile{ tileset.getTileSize(), std::move(layers) };
}

std::vector<ska::LayerRenderablePtr> ska::TileWorldLoaderAggregate::loadGraphics(Tileset& tileset, const unsigned int blockSize) const {
	std::vector<LayerRenderablePtr> renderables;
	if(m_loaders.empty()) {
		return renderables;
	}

	auto index = 0u;
	while (index < m_layerFixed) {
		renderables.push_back(std::make_unique<LayerMonoRenderable>(m_loaders[index++]->loadAnimations(tileset), tileset.getTexture(), blockSize));
	}
	while (index < m_loaders.size()) {
		auto layer = std::make_unique<LayerYIndexRenderable>(m_loaders[index++]->loadAnimations(tileset), tileset.getTexture(), blockSize, index);
		renderables.push_back(std::move(layer));
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
