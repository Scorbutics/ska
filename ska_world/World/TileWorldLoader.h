#pragma once
#include "LayerRenderable.h"
#include "LayerEvent.h"

namespace ska {
	class Tileset;
	class CollisionProfile;
	class TileWorld;

	class TileWorldLoader : public MovableNonCopyable {
	public:
		virtual CollisionProfile loadPhysics(Tileset& tileset) const = 0;
		virtual std::vector<LayerRenderablePtr> loadGraphics(Tileset& tileset, unsigned int blockSize) const = 0;
		virtual std::vector<LayerEventPtr> loadEvents() const = 0;
		virtual const std::string& getName() const = 0;
	};

}
