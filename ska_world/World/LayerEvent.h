#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "Utils/Refreshable.h"

namespace ska {
	class World;
	class LayerEvent : public Refreshable {
	public:
		LayerEvent(World& world);
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
		void changeLevel(const std::string& nomFichier);
		std::string getPath(int ligne) const;

	private:
		std::string m_nomFichier, m_chipsetname;
		int m_nbrLignes;
		std::vector<int> m_coordBX, m_coordBY, m_ID, m_trigger, m_solide;
		std::vector<std::string> m_action, m_param, m_path;
		World& m_world;
	};
	using LayerEPtr = std::unique_ptr<LayerEvent>;
}

