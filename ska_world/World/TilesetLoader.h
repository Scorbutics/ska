#pragma once
#include <optional>
#include "Utils/MovableNonCopyable.h"
#include "Physic/Tile.h"
#include "Utils/Vector2.h"
#include "Graphic/System/AbstractGraphicSystem.h"
#include "TileRenderable.h"

namespace ska {
	class TilesetLoader : public MovableNonCopyable {
	public:
		virtual Vector2<Tile> loadPhysics() const = 0;
		virtual Texture loadTexture() const = 0;
		virtual Vector2<std::optional<Animation>> loadAnimations(unsigned int blockSize) const = 0;
		virtual std::string getName() const = 0;
	};
}
