#pragma once
#include <string>
#include "TileWorldLoader.h"
#include "Tileset.h"
#include "LayerRenderable.h"
#include "LayerLoader.h"

namespace ska {
	class CollisionProfile;

	class TileWorldLoaderImage : public TileWorldLoader {
	public:
		TileWorldLoaderImage (const std::string& corrFileName, std::string levelPath);
				
		CollisionProfile loadPhysics(Tileset& tileset) const override;
		std::vector<LayerRenderablePtr> loadGraphics(Tileset& tileset, unsigned int blockSize) const override;
		const std::string& getName() const override;

	private:
		const TilesetCorrespondanceMapper m_correspondanceMapper;
		const std::string m_levelPath;
		std::vector<std::unique_ptr<LayerLoader>> m_loaders{};
		
	};
}
