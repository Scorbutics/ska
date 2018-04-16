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
		LayerEvent(const LayerEventLoaderText& loader);
		LayerEvent(LayerEvent&&) = default;
		void operator=(const LayerEvent&) = delete;

		virtual void refresh(unsigned int ellapsedTime) override;

		int getBlocX(int ligne) const;
		int getBlocY(int ligne) const;
		int getID(int ligne) const;
		std::string getParam(int ligne) const;
		int getTrigger(int ligne) const;
		std::string getAction(int ligne) const;
		int getSolide(int ligne) const;
		int getNbrLignes() const;

		std::string getPath(int ligne) const;

	private:
		void load(const LayerEventLoader& loader);

		std::string m_fileName;
		std::string m_chipsetname;

        std::vector<BlockEvent> m_events;
	};
}

