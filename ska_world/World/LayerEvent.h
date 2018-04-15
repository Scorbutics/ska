#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "Utils/Refreshable.h"

namespace ska {
	class LayerEventLoader;

	class LayerEvent : public Refreshable {
	public:
		LayerEvent();
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
		void load(LayerEventLoader& loader);

		std::string m_nomFichier;
		std::string m_chipsetname;
		int m_nbrLignes;

		std::vector<int> m_coordBX;
		std::vector<int> m_coordBY;
		std::vector<int> m_ID;
		std::vector<int> m_trigger;
		std::vector<int> m_solide;
		
		std::vector<std::string> m_action;
		std::vector<std::string> m_param;
		std::vector<std::string> m_path;
	};
	using LayerEPtr = std::unique_ptr<LayerEvent>;
}

