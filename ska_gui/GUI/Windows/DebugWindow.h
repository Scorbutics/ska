#pragma once
#include <functional>
#include <unordered_map>

#include "Utils/SubObserver.h"
#include "Point.h"
#include "Draw/DrawableContainer.h"
#include "Data/Events/GameEventDispatcher.h"
#include "MoveableWindow.h"

namespace ska {
	class GUI;
	class Widget;
	class ImageRectangle;
	class Label;

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
		void editText(ska::Label& w, std::string text);

		std::unordered_map<std::string, DebugEntryLabel> m_debugEntries;
		std::unordered_map<std::string, Widget*> m_labels;
		GUI& m_gui;
		GameEventDispatcher& m_ged;
		MoveableWindow<>* m_dbgWindow = nullptr;
		ImageRectangle* m_background = nullptr;
		Label* m_title = nullptr;

		static constexpr auto VERTICAL_OFFSET = 5;
		static constexpr auto HORIZONTAL_OFFSET = 5;
		static constexpr auto FONT_SIZE = 14;
		static constexpr uint8_t OPACITY = 120;
	};
}
