#pragma once
#include "Base/Values/MovableNonCopyable.h"
#include "Base/Containers/Vector2.h"
#include <optional>

namespace ska {
	class Tileset;
	struct TileAnimation;

	class LayerLoader : public MovableNonCopyable {
	public :
		virtual Vector2<std::optional<Tile>> loadPhysics(const Tileset& chipset) const = 0;
		virtual Vector2<TileAnimation*> loadAnimations(Tileset& chipset) const = 0;
		~LayerLoader() override = default;
	};

}
