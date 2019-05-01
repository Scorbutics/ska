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
                        std::vector<std::unique_ptr<LayerEventLoader>> eventLoaders);

		CollisionProfile loadPhysics(Tileset& tileset) const override;
		std::vector<LayerRenderablePtr> loadGraphics(Tileset& tileset, unsigned int blockSize) const override;
		std::vector<LayerEventPtr> loadEvents(unsigned int widthLayer, unsigned int heightLayer) const override;
		const std::string& getName() const override;

	private:
		const std::size_t m_layerFixed;
		const std::string m_levelPath;
		const std::vector<std::unique_ptr<LayerLoader>> m_loaders;
		const std::vector<std::unique_ptr<LayerEventLoader>> m_eventLoaders;
	};
}
