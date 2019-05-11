#pragma once
#include "Base/Values/MovableNonCopyable.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class TilesetEventLoader : public MovableNonCopyable {
	public:
		virtual std::vector<ScriptSleepComponent> load() const = 0;
	};

}
