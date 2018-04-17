#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

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
		LayerEvent(const LayerEventLoaderText& loader);
		LayerEvent(LayerEvent&&) = default;
        ~LayerEvent() override = default;

        void load(const LayerEventLoader& loader);

		virtual void refresh(unsigned int ellapsedTime) override;

		int getBlocX(int ligne) const;
		int getBlocY(int ligne) const;
		int getID(int ligne) const;
		std::string getParam(int ligne) const;
		int getTrigger(int ligne) const;
		std::string getAction(int ligne) const;
		int getSolid(int ligne) const;
		int getLines() const;

		std::string getPath(int ligne) const;

	private:
		std::string m_fileName;
		std::string m_chipsetname;

        std::vector<BlockEvent> m_events;
	};

	using LayerEventPtr = std::unique_ptr<LayerEvent>;
}

