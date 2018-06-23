#pragma once

#include <string>
#include <memory>

#include "Utils/Vector2.h"
#include "Utils/MovableNonCopyable.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Graphic/AnimatedTexture.h"
#include "ECS/Basics/Script/ScriptGlobalData.h"

namespace ska {
    class LayerEventLoader;

	class LayerEvent :
        public MovableNonCopyable {
	public:
	    LayerEvent() = default;
		LayerEvent(const LayerEventLoader& loader, unsigned int width, unsigned int height);
		LayerEvent(LayerEvent&&) = default;
        ~LayerEvent() override = default;

		ScriptPack& getScripts(const ska::Point<int>& coordinates);
		ScriptGlobalPack& getAutoScript();

	private:
		void forcesAutoOnGlobalScripts();
	    void load(const LayerEventLoader& loader, unsigned int width, unsigned int height);

		std::string m_fileName;
		std::string m_chipsetname;

        Vector2<ScriptPack> m_events;
		ScriptGlobalPack m_autoEvents;
	};

	using LayerEventPtr = std::unique_ptr<LayerEvent>;
}

