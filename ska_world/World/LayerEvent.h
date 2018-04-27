#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "Utils/Vector2.h"
#include "Utils/Refreshable.h"
#include "Utils/MovableNonCopyable.h"
#include "BlockEvent.h"

namespace ska {
	class LayerEventLoader;
    class LayerEventLoaderText;

	class LayerEvent :
        public MovableNonCopyable,
        public Refreshable {
	public:
	    LayerEvent() = default;
	    LayerEvent(std::string levelPath, ska::Vector2<std::optional<BlockEvent>> blocks);
		LayerEvent(const LayerEventLoaderText& loader, unsigned int width, unsigned int height);
		LayerEvent(LayerEvent&&) = default;
        ~LayerEvent() override = default;

		virtual void refresh(unsigned int ellapsedTime) override;

	private:
	    void load(const LayerEventLoader& loader, unsigned int width, unsigned int height);

		std::string m_fileName;
		std::string m_chipsetname;

        ska::Vector2<std::optional<BlockEvent>> m_events;
	};

	using LayerEventPtr = std::unique_ptr<LayerEvent>;
}

