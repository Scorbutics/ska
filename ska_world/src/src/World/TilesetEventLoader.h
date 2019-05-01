#pragma once
#include "Utils/MovableNonCopyable.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class TilesetEventLoader : public MovableNonCopyable {
	public:
		virtual std::vector<ScriptSleepComponent> load() const = 0;
	};

}
