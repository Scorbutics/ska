#pragma once
#include <optional>
#include "Utils/MovableNonCopyable.h"
#include "Tile.h"
#include "Utils/Vector2.h"
#include "Graphic/Texture.h"
#include "Graphic/Animation.h"
#include "TileAnimation.h"

namespace ska {
	class TilesetLoader : public MovableNonCopyable {
	public:
		virtual Vector2<Tile> loadPhysics() const = 0;
		virtual Texture loadGraphics() const = 0;
		virtual Vector2<TileAnimation> loadAnimations(unsigned int blockSize) const = 0;
		virtual std::string getName() const = 0;
	};
}
