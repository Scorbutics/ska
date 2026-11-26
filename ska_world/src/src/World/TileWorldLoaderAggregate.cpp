#include "World/__internalConfig/LoggerConfig.h"
#include "TileWorldLoaderAggregate.h"
#include "Layer.h"
#include "LayerLoader.h"
#include "LayerRenderable.h"
#include "TileWorld.h"
#include "LayerEventLoaderText.h"
#include "LayerDepthRenderable.h"

ska::TileWorldLoaderAggregate::TileWorldLoaderAggregate(
                                                std::string levelPath,
												const std::size_t layerFixed,
                                                std::vector<std::unique_ptr<LayerLoader>> loaders,
                                                std::vector<std::unique_ptr<LayerEventLoader>> eventLoaders,
												const int sortedLayersPriority) :
	m_layerFixed(layerFixed > loaders.size() ? loaders.size() : layerFixed),
	m_sortedLayersPriority(sortedLayersPriority),
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
	for (auto index = 0u; index < m_loaders.size(); index++) {
		const auto& loader = *m_loaders[index];
		// The base of each tile (its properties in the tileset) ; in the layers after the fixed ones, a tile with no base
		// (a tileset with no depth in its properties) stands on its own block : drawn sorted with the characters by its row
		const auto tiles = loader.loadPhysics(tileset);
		auto bases = Vector2<int>{};
		bases.reserve(tiles.lineSize(), tiles.lineSize() == 0 ? 0 : tiles.size() / tiles.lineSize());
		for (const auto& tile : tiles) {
			const auto base = tile.has_value() ? tile->properties.base : -1;
			bases.push_back(base < 0 && tile.has_value() && index >= m_layerFixed ? 0 : base);
		}
		renderables.push_back(std::make_unique<LayerDepthRenderable>(loader.loadAnimations(tileset), bases, tileset.getTexture(), blockSize,
			FlatLayersPriority + static_cast<int>(index), m_sortedLayersPriority + static_cast<int>(index)));
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
