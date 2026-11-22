#pragma once
#include <string>
#include "TileWorldLoader.h"
#include "Tileset.h"
#include "LayerMonoRenderable.h"
#include "CollisionProfile.h"
#include "LayerLoader.h"
#include "LayerEventLoader.h"

namespace ska {
	class CollisionProfile;

	class TileWorldLoaderAggregate : public TileWorldLoader {
	public:
		TileWorldLoaderAggregate (
                        std::string levelPath,
						std::size_t layer,
                        std::vector<std::unique_ptr<LayerLoader>> loaders,
                        std::vector<std::unique_ptr<LayerEventLoader>> eventLoaders,
						int sortedLayersPriority = 0);

		CollisionProfile loadPhysics(Tileset& tileset) const override;
		std::vector<LayerRenderablePtr> loadGraphics(Tileset& tileset, unsigned int blockSize) const override;
		std::vector<LayerEventPtr> loadEvents(unsigned int widthLayer, unsigned int heightLayer) const override;
		const std::string& getName() const override;

	private:
		const std::size_t m_layerFixed;
		// Priority added to the layers drawn row by row (the layers after the fixed ones) : 0, they are sorted with the
		// characters (by row) ; above the priorities of the characters, they are always drawn over them
		const int m_sortedLayersPriority;
		const std::string m_levelPath;
		const std::vector<std::unique_ptr<LayerLoader>> m_loaders;
		const std::vector<std::unique_ptr<LayerEventLoader>> m_eventLoaders;
	};
}
