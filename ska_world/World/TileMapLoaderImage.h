#pragma once
#include <string>
#include "TileMapLoader.h"
#include "Tileset.h"
#include "Layer.h"
#include "LayerRenderable.h"

namespace ska {
	class CollisionProfile;

	class TileMapLoaderImage : public TileMapLoader {
	public:
		TileMapLoaderImage (std::string levelPath, std::string chipsetPath, std::string chipsetCorrespondanceFileName);
		std::pair<std::unique_ptr<ska::Layer>, std::unique_ptr<ska::LayerRenderable>> loadLayer(int blockSize, Tileset& tileset, const std::string& layerFileName);
		std::unique_ptr<ska::Tileset> load(int blockSize, CollisionProfile& profile, std::vector<LayerRenderablePtr>& renderables) override;
	
	private:
		std::string m_chipsetCorrespondanceFileName;
		std::string m_levelPath;
		std::string m_chipsetPath;
	};
}
