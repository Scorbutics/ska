#pragma once
#include "Utils/Vector2.h"
#include "BlockEvent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class LayerEventLoader {
	public:
		virtual ~LayerEventLoader() = default;

		virtual Vector2<ScriptPack> loadPositioned(unsigned int width, unsigned int height) const = 0;
		virtual ScriptPack loadGlobal() const = 0;
		virtual const std::string& getName() const = 0;
	};
}
