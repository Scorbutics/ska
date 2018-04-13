#pragma once
#include "Utils/MovableNonCopyable.h"
#include "Physic/Tile.h"
#include "Utils/Vector2.h"
#include "Graphic/System/AbstractGraphicSystem.h"
#include "TileRenderable.h"

namespace ska {
	class TilesetLoader : public MovableNonCopyable {
	public:
		virtual Vector2<Tile> loadPhysics() const = 0;
		virtual std::pair<Texture, Vector2<TileRenderable>> loadGraphics(unsigned int blockSize) const = 0;
		virtual std::string getName() const = 0;
	};
}
