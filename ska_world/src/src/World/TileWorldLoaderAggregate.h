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

		// The flat tiles of the layers (TileProperties::base -1) : under everything (the layer i at FlatLayersPriority + i)
		static constexpr int FlatLayersPriority = -1000;

	private:
		// The first layers (the ground) : their tiles with no base are flat ; after them, a tile with no base stands on
		// its own block (the tilesets with no depth in their properties : the top layer sorted by row)
		const std::size_t m_layerFixed;
		// Priority of the base line of the standing tiles of the layers (LayerDepthRenderable) : the tiles whose base is
		// on the block row r are drawn at m_sortedLayersPriority + the layer index + r * the block size, sorted with the
		// characters (their priority : their feet on the ground, ska::GraphicSystem)
		const int m_sortedLayersPriority;
		const std::string m_levelPath;
		const std::vector<std::unique_ptr<LayerLoader>> m_loaders;
		const std::vector<std::unique_ptr<LayerEventLoader>> m_eventLoaders;
	};
}
