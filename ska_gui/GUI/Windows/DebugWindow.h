#pragma once
#include <functional>
#include <unordered_map>

#include "Utils/SubObserver.h"
#include "Point.h"
#include "Draw/DrawableContainer.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	class GUI;
	class Widget;

	struct DebugEntryLabel {
		using DebugWindowRefreshFunction = std::function<std::string()>;

		std::string label;
		DebugWindowRefreshFunction updater;
		unsigned int delay = 500u;
		unsigned int count = 0u;
	};

	class DebugWindow : 
		public SubObserver<ska::DebugGUIEvent> {

	public:
		DebugWindow(GUI& gui, GameEventDispatcher& ged);
		void refresh(unsigned int ellapsedTime);
		virtual ~DebugWindow() = default;

	private:
		bool onDebugGUIEvent(DebugGUIEvent& debugGuiEvent);

		std::unordered_map<std::string, DebugEntryLabel> m_debugEntries;
		std::unordered_map<std::string, Widget*> m_labels;
		GUI& m_gui;
		GameEventDispatcher& m_ged;
	};
}
