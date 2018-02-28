#pragma once
#include "BlockRenderable.h"
#include "Physic/Block.h"
#include "Utils/Vector2.h"

namespace ska {
	struct LayerData {
		Vector2<Block*> physics;
		Vector2<BlockRenderable*> graphics;
	};
}
