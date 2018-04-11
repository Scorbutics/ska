#pragma once
#include "LayerRenderable.h"
#include "Tileset.h"

namespace ska {
	class CollisionProfile;
	class TileWorld;

	class TileMapLoader {
	public:
		virtual ~TileMapLoader() = default;
		virtual std::unique_ptr<ska::Tileset> load(int blockSize, CollisionProfile& profile, std::vector<LayerRenderablePtr>& renderables) = 0;
	};

}
