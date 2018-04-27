#pragma once
#include "LayerRenderable.h"
#include "LayerEvent.h"
#include "CollisionProfile.h"

namespace ska {
	class Tileset;
	class TileWorld;

	class TileWorldLoader : public MovableNonCopyable {
	public:
		virtual CollisionProfile loadPhysics(Tileset& tileset) const = 0;
		virtual std::vector<LayerRenderablePtr> loadGraphics(Tileset& tileset, unsigned int blockSize) const = 0;
		virtual std::vector<LayerEventPtr> loadEvents(unsigned int widthLayer, unsigned int heightLayer) const = 0;
		virtual const std::string& getName() const = 0;
	};

}
