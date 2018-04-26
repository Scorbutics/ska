#pragma once
#include "Utils/MovableNonCopyable.h"
#include "Utils/Vector2.h"
#include <optional>
#include "Graphic/Animation.h"

namespace ska {
	class Tileset;

	class LayerLoader : public MovableNonCopyable {
	public :
		virtual Vector2<std::optional<Tile>> loadPhysics(Tileset& chipset) const = 0;
		virtual Vector2<Animation*> loadAnimations(Tileset& chipset) const = 0;
		~LayerLoader() override = default;
	};

}
